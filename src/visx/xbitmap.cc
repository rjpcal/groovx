///////////////////////////////////////////////////////////////////////
//
// xbitmap.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Sep  7 14:37:04 1999
// written: Tue Oct 12 18:35:00 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef XBITMAP_CC_DEFINED
#define XBITMAP_CC_DEFINED

#include "xbitmap.h"

#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>

#include "toglconfig.h"
#include "objtogl.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

namespace {
  const string ioTag = "XBitmap";
  bool class_inited = false;

  GC gfx_context_white, gfx_context_black;
  Display* display;
  Window win;
  int screen;
  Visual* visual;

  void init_class() {
  DOTRACE("<xbitmap.cc>::init_class");
    ToglConfig* config = ObjTogl::theToglConfig();

	 display = config->getX11Display();
	 win = config->getX11Window();
	 screen = config->getX11ScreenNumber();

    XGCValues gc_values;
	 gc_values.foreground = XWhitePixel(display,screen);
	 gc_values.background = XBlackPixel(display,screen);
	 
	 unsigned long gc_valuemask = GCForeground | GCBackground;
	 
	 gfx_context_white = XCreateGC(display,win,gc_valuemask,&gc_values);

	 gc_values.foreground = XBlackPixel(display,screen);
	 gc_values.background = XWhitePixel(display,screen);

	 gfx_context_black = XCreateGC(display,win,gc_valuemask,&gc_values);

	 XWindowAttributes xwa;
	 Status status = XGetWindowAttributes(display, win, &xwa);
	 visual = xwa.visual;

	 class_inited = true;
  }
}


//////////////
// creators //
//////////////

XBitmap::XBitmap() :
  Bitmap()
{
DOTRACE("XBitmap::XBitmap");
  init();
}

XBitmap::XBitmap(const char* filename) :
  Bitmap(filename)
{
DOTRACE("XBitmap::XBitmap");
  init();
}

XBitmap::XBitmap(istream& is, IOFlag flag) :
  Bitmap()
{
DOTRACE("XBitmap::XBitmap");
  init();
  deserialize(is, flag);
}

void XBitmap::init() {
DOTRACE("XBitmap::init");

  if (!class_inited) init_class();

  GrObj::setRenderMode(GROBJ_DIRECT_RENDER);
  GrObj::setUnRenderMode(GROBJ_CLEAR_BOUNDING_BOX);
  Bitmap::setUsingZoom(false);

  itsImage = NULL;
}

XBitmap::~XBitmap() {
DOTRACE("XBitmap::~XBitmap");
  if (itsImage) XFree(itsImage);
}

void XBitmap::serialize(ostream& os, IOFlag flag) const {
DOTRACE("XBitmap::serialize");
  char sep = ' ';
  if (flag & TYPENAME) { os << ioTag << sep; }

  if (os.fail()) throw OutputError(ioTag);

  if (flag & BASES) { Bitmap::serialize(os, flag); }
}

void XBitmap::deserialize(istream& is, IOFlag flag) {
DOTRACE("XBitmap::deserialize");
  if (flag & TYPENAME) { IO::readTypename(is, ioTag); }

  if (is.fail()) throw InputError(ioTag);

  if (flag & BASES) { Bitmap::deserialize(is, flag); }
}

int XBitmap::charCount() const {
DOTRACE("XBitmap::charCount");
  return 128;
}

/////////////
// actions //
/////////////

void XBitmap::bytesChangeHook(unsigned char* theBytes, int width, int height,
										int bits_per_pixel, int byte_alignment) {
DOTRACE("XBitmap::bytesChangeHook");
  if (itsImage) {
	 XFree(itsImage);
	 itsImage = 0;
  }

  if (theBytes != 0) {
	 int format;
	 if (bits_per_pixel == 1) {
		format = XYBitmap;
	 }
	 else {
		format = ZPixmap;
	 }
	 itsImage =
		XCreateImage(display, visual,
						 bits_per_pixel, /* bit depth */
						 format, /* format = XYBitmap, XYPixmap, ZPixmap */
						 0, /* offset */
						 reinterpret_cast<char*>(theBytes),
						 width, height,
						 byte_alignment*8, /* bitmap_pad */
						 0); /* bytes_per_line */
  }
}

void XBitmap::doUndraw(int winRasterX, int winRasterY,
							  int winWidthX, int winHeightY) const {
DOTRACE("XBitmap::doUndraw");
  glPushAttrib(GL_SCISSOR_BIT);
    glEnable(GL_SCISSOR_TEST);
      glScissor(winRasterX, winRasterY, winWidthX, winHeightY);
		glClear(GL_COLOR_BUFFER_BIT);
	 glDisable(GL_SCISSOR_TEST);
  glPopAttrib();	 
}

void XBitmap::doRender(unsigned char* /* bytes */,
							  double x_pos,
							  double y_pos,
							  int width,
							  int height,
							  int /* bits_per_pixel */,
							  int /* byte_alignment */,
							  double /* zoom_x */,
							  double /* zoom_y */) const {
DOTRACE("XBitmap::doRender");

  // Check if we have an image to display 
  if (itsImage == NULL) return; 

  // Calculate GL window coordinates of lower left corner of image
  int screenX, screenY;
  getScreenFromWorld(x_pos, y_pos, screenX, screenY);

  // Calculate GL window coordinates for upper left corner of image
  screenY += height;

  // Calculate X11 window coordinates for upper left corner of image
  // (X11 coordinates put 0 at the top of the window, while GL
  // coordinates put 0 at the bottom of the window)
  XWindowAttributes xwa;
  XGetWindowAttributes(display, win, &xwa);
  screenY = xwa.height - screenY;

  // Draw the image
  glXWaitGL();
  XPutImage(display, win, gfx_context_black, itsImage,
				0, 0, screenX, screenY,
				width, height);
  glXWaitX();
}

static const char vcid_xbitmap_cc[] = "$Header$";
#endif // !XBITMAP_CC_DEFINED
