///////////////////////////////////////////////////////////////////////
//
// xbmaprenderer.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Dec  1 17:22:34 1999
// written: Wed Aug  8 12:48:24 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef XBMAPRENDERER_CC_DEFINED
#define XBMAPRENDERER_CC_DEFINED

#include "xbmaprenderer.h"

#include "bmapdata.h"
#include "point.h"

#include "gwt/canvas.h"

#include "util/error.h"

#include <tk.h>
#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glx.h>

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

namespace
{
  bool x11_ready = false;

  GC gfx_context_white, gfx_context_black;
  Display* display;
  Window win;
  int screen;
  Visual* visual;

  Tk_Window theTkWin = 0;

  void setupX11Stuff()
  {
  DOTRACE("{xbmaprenderer.cc}::setupX11Stuff");
    if (x11_ready) return;

    if (theTkWin == 0)
      throw Util::Error(
          "must call XBitmap::initClass before using an XBmapRenderer");

    if (!Tk_IsMapped(reinterpret_cast<Tk_FakeWin *>(theTkWin)))
      throw Util::Error(
          "the main window must be mapped before using an XBmapRenderer");

    display = Tk_Display(reinterpret_cast<Tk_FakeWin *>(theTkWin));
    win = Tk_WindowId(reinterpret_cast<Tk_FakeWin *>(theTkWin));
    screen = Tk_ScreenNumber(reinterpret_cast<Tk_FakeWin *>(theTkWin));

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
    if (status == 0)
        throw Util::Error("couldn't get X11 window attributes");
    visual = xwa.visual;

    x11_ready = true;
  }
}

void XBmapRenderer::initClass(Tk_Window tkwin)
{
DOTRACE("XBmapRenderer::initClass");
  theTkWin = tkwin;
}

XBmapRenderer::XBmapRenderer() :
  itsIsCurrent(false),
  itsImage(0)
{
DOTRACE("XBmapRenderer::XBmapRenderer");
}

XBmapRenderer::~XBmapRenderer()
{
DOTRACE("XBmapRenderer::~XBmapRenderer");
  if (itsImage) XFree(itsImage);
}

void XBmapRenderer::doRender(GWT::Canvas& canvas,
                             const BmapData& data,
                             const Point<double>& world_pos,
                             const Point<double>& /* zoom */) const
{
DOTRACE("XBmapRenderer::doRender");

  setupX11Stuff();

  // Update the cached image if necessary
  if (!itsIsCurrent) update(data);

  // Check if we have an image to display
  if (itsImage == NULL) return;

  // Calculate GL window coordinates of lower left corner of image
  Point<int> screen_pos = canvas.getScreenFromWorld(world_pos);

  DebugEval(world_pos.x()); DebugEval(world_pos.y());
  DebugEval(screen_pos.x()); DebugEvalNL(screen_pos.y());

  // Calculate GL window coordinates for upper left corner of image
  screen_pos.y() += data.height();

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
            data.width(), data.height());
  glXWaitX();
}

void XBmapRenderer::notifyBytesChanged() const
{
DOTRACE("XBmapRenderer::notifyBytesChanged");
  itsIsCurrent = false;
}

void XBmapRenderer::update(const BmapData& data) const
{
DOTRACE("XBmapRenderer::update");

  if (itsIsCurrent) return;

  setupX11Stuff();

  if (itsImage)
    {
      XFree(itsImage);
      itsImage = 0;
    }

  if (data.bytesPtr() != 0)
    {
      int format = (data.bitsPerPixel() == 1) ? XYBitmap : ZPixmap;

      itsImage =
        XCreateImage(display, visual,
                     data.bitsPerPixel(), /* bit depth */
                     format, /* format = XYBitmap, XYPixmap, ZPixmap */
                     0, /* offset */
                     reinterpret_cast<char*>(data.bytesPtr()),
                     data.width(), data.height(),
                     data.byteAlignment()*8, /* bitmap_pad */
                     0); /* bytes_per_line */
    }

  itsIsCurrent = true;
}

static const char vcid_xbmaprenderer_cc[] = "$Header$";
#endif // !XBMAPRENDERER_CC_DEFINED
