///////////////////////////////////////////////////////////////////////
//
// xbmaprenderer.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Dec  1 17:22:34 1999
// written: Thu Nov 21 09:46:16 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef XBMAPRENDERER_CC_DEFINED
#define XBMAPRENDERER_CC_DEFINED

#include "xbmaprenderer.h"

#include "gfx/canvas.h"

#include "gx/bmapdata.h"
#include "gx/vec2.h"

#include "util/error.h"

#include <GL/glx.h>

#include "util/trace.h"
#include "util/debug.h"

void XBmapRenderer::doRender(Display* display,
                             Window win,
                             int screen,
                             Gfx::Canvas& canvas,
                             const Gfx::BmapData& data,
                             const Gfx::Vec2<double>& world_pos,
                             const Gfx::Vec2<double>& /* zoom */)
{
DOTRACE("XBmapRenderer::doRender");

  XWindowAttributes xwa;
  Status status = XGetWindowAttributes(display, win, &xwa);
  if (status == 0)
    {
      throw Util::Error("couldn't get X11 window attributes");
    }
  Visual* visual = xwa.visual;

  int format = (data.bitsPerPixel() == 1) ? XYBitmap : ZPixmap;

  data.setRowOrder(Gfx::BmapData::TOP_FIRST);

  // Calculate GL window coordinates of lower left corner of image
  Gfx::Vec2<int> screen_pos = canvas.screenFromWorld(world_pos);

  dbgEval(3, world_pos.x()); dbgEval(3, world_pos.y());
  dbgEval(3, screen_pos.x()); dbgEvalNL(3, screen_pos.y());

  // Calculate GL window coordinates for upper left corner of image
  screen_pos.y() += data.height();

  // Calculate X11 window coordinates for upper left corner of image
  // (X11 coordinates put 0 at the top of the window, while GL
  // coordinates put 0 at the bottom of the window)
  screen_pos.y() = xwa.height - screen_pos.y();

  // Create the XImage
  XImage* image = XCreateImage(display, visual,
                               data.bitsPerPixel(), /* bit depth */
                               format, /* format = XYBitmap, XYPixmap, ZPixmap */
                               0, /* offset */
                               reinterpret_cast<char*>(data.bytesPtr()),
                               data.width(), data.height(),
                               data.byteAlignment()*8, /* bitmap_pad */
                               0); /* bytes_per_line */

  if (image == NULL)
    {
      throw Util::Error("couldn't create an XImage");
    }

  // Create a graphics context
  XGCValues gc_values;

  gc_values.foreground = XBlackPixel(display, screen);
  gc_values.background = XWhitePixel(display, screen);

  unsigned long gc_valuemask = GCForeground | GCBackground;

  GC gfx_context_black = XCreateGC(display, win, gc_valuemask, &gc_values);

  // Draw the image
  glXWaitGL();
  XPutImage(display, win, gfx_context_black, image,
            0, 0, screen_pos.x(), screen_pos.y(),
            data.width(), data.height());
  glXWaitX();

  XFreeGC(display, gfx_context_black);
  XFree(image);
}

static const char vcid_xbmaprenderer_cc[] = "$Header$";
#endif // !XBMAPRENDERER_CC_DEFINED