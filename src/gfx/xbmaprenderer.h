///////////////////////////////////////////////////////////////////////
//
// xbmaprenderer.h
//
// Copyright (c) 1999-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Wed Dec  1 17:19:23 1999
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef XBMAPRENDERER_H_DEFINED
#define XBMAPRENDERER_H_DEFINED

#include <X11/Xlib.h>

namespace Gfx
{
  class BmapData;
  class Canvas;
  template <class V> class Vec2;
}

/// XBmapRenderer is "in between jobs"... needs to be merged into GLCanvas.
namespace XBmapRenderer
{
   void doRender(Display* display,
                 Window win,
                 int screen,
                 Gfx::Canvas& canvas,
                 const Gfx::BmapData& data,
                 const Gfx::Vec2<double>& world_pos,
                 const Gfx::Vec2<double>& zoom);
};

//---------------------------------------------------------------------
//
// Implementation
//
//---------------------------------------------------------------------

#include "gfx/canvas.h"

#include "gx/bmapdata.h"
#include "gx/vec2.h"

#include "util/error.h"

#include <GL/glx.h>

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER

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

static const char vcid_xbmaprenderer_h[] = "$Header$";
#endif // !XBMAPRENDERER_H_DEFINED
