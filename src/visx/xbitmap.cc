///////////////////////////////////////////////////////////////////////
//
// xbitmap.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Sep  7 14:37:04 1999
// written: Wed Sep  8 13:08:41 1999
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

#include "pbm.h"
#include "toglconfig.h"
#include "objtogl.h"

#define LOCAL_TRACE
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
  DOTRACE("<bitmap.cc>::init_class");
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

  GrObj::setUsingCompile(false);
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

  if (flag & BASES) { GrObj::serialize(os, flag); }
}

void XBitmap::deserialize(istream& is, IOFlag flag) {
DOTRACE("XBitmap::deserialize");
  if (flag & TYPENAME) { IO::readTypename(is, ioTag); }

  if (is.fail()) throw InputError(ioTag);

  if (flag & BASES) { GrObj::deserialize(is, flag); }
}

int XBitmap::charCount() const {
DOTRACE("XBitmap::charCount");
  return 128;
}

/////////////
// actions //
/////////////

void XBitmap::bytesChangeHook(unsigned char* theBytes, int width, int height) {
DOTRACE("XBitmap::bytesChangeHook");
  if (itsImage) {
	 XFree(itsImage);
	 itsImage = 0;
  }

  if (theBytes != 0) {
	 itsImage = XCreateImage(display, visual, 1, XYBitmap, 0,
									 reinterpret_cast<char*>(theBytes),
									 width, height, 8, 0);
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
							  double /* zoom_x */,
							  double /* zoom_y */) const {
DOTRACE("XBitmap::doRender");

  // Check if we have an image to display 
  if (itsImage == NULL) return; 

  // Calculate the correct window coordinates for the location of
  // (itsRasterX, itsRasterY) in GL coordinates
  GLdouble mv_matrix[16];
  glGetDoublev(GL_MODELVIEW_MATRIX, mv_matrix);

  GLdouble proj_matrix[16];
  glGetDoublev(GL_PROJECTION_MATRIX, proj_matrix);

  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  GLdouble winX, winY, winZ;
  GLint status = gluProject(x_pos, y_pos, 0.0,
									 mv_matrix, proj_matrix, viewport,
									 &winX, &winY, &winZ);

  if (status == GL_FALSE)
	 throw ErrorWithMsg("XBitmap::grRender(): gluProject error");

  // Draw the image
  if (itsImage != NULL) {
	 glXWaitGL();
	 XPutImage(display, win, gfx_context_black, itsImage,
				  0, 0, int(winX), int(winY),
				  width, height);
	 glXWaitX();
  }
}

static const char vcid_xbitmap_cc[] = "$Header$";
#endif // !XBITMAP_CC_DEFINED
