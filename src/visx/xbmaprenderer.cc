///////////////////////////////////////////////////////////////////////
//
// xbmaprenderer.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Dec  1 17:22:34 1999
// written: Wed Dec  1 17:51:39 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef XBMAPRENDERER_CC_DEFINED
#define XBMAPRENDERER_CC_DEFINED

#include "xbmaprenderer.h"

#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>

#include "canvas.h"
#include "error.h"
#include "point.h"
#include "toglconfig.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

namespace {
  bool class_inited = false;

  GC gfx_context_white, gfx_context_black;
  Display* display;
  Window win;
  int screen;
  Visual* visual;
}

void XBmapRenderer::initClass(const ToglConfig* config) {
DOTRACE("XBmapRenderer::initClass");
  if (class_inited) return;

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

XBmapRenderer::XBmapRenderer() {
  if (!class_inited) throw ErrorWithMsg("XBitmap::initClass not yet called");

  itsImage = 0;
}

XBmapRenderer::~XBmapRenderer() {
  if (itsImage) XFree(itsImage);
}

void XBmapRenderer::doRender(unsigned char* /* bytes */,
									  double x_pos,
									  double y_pos,
									  int width,
									  int height,
									  int /* bits_per_pixel */,
									  int /* byte_alignment */,
									  double /* zoom_x */,
									  double /* zoom_y */) const {
DOTRACE("XBmapRenderer::doRender");

  // Check if we have an image to display 
  if (itsImage == NULL) return; 

  // Calculate GL window coordinates of lower left corner of image
  Point<int> screen_pos =
	 Canvas::theCanvas().getScreenFromWorld(Point<double>(x_pos, y_pos));

  // Calculate GL window coordinates for upper left corner of image
  screen_pos.y() += height;

  // Calculate X11 window coordinates for upper left corner of image
  // (X11 coordinates put 0 at the top of the window, while GL
  // coordinates put 0 at the bottom of the window)
  XWindowAttributes xwa;
  XGetWindowAttributes(display, win, &xwa);
  screen_pos.y() = xwa.height - screen_pos.y();

  // Draw the image
  glXWaitGL();
  XPutImage(display, win, gfx_context_black, itsImage,
				0, 0, screen_pos.x(), screen_pos.y(),
				width, height);
  glXWaitX();
}

void XBmapRenderer::doUndraw(int winRasterX, int winRasterY,
									  int winWidthX, int winHeightY) const {
DOTRACE("XBmapRenderer::doUndraw");
  glPushAttrib(GL_SCISSOR_BIT);
    glEnable(GL_SCISSOR_TEST);
      glScissor(winRasterX, winRasterY, winWidthX, winHeightY);
		glClear(GL_COLOR_BUFFER_BIT);
	 glDisable(GL_SCISSOR_TEST);
  glPopAttrib();	 
}

void XBmapRenderer::bytesChangeHook(unsigned char* theBytes,
												int width,
												int height,
												int bits_per_pixel,
												int byte_alignment) {
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

static const char vcid_xbmaprenderer_cc[] = "$Header$";
#endif // !XBMAPRENDERER_CC_DEFINED
