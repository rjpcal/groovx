///////////////////////////////////////////////////////////////////////
//
// xbmaprenderer.h
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Wed Dec  1 17:19:23 1999
// commit: $Id$
// $HeadURL$
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

namespace geom
{
  template <class V> class vec2;
}

namespace media
{
  class bmap_data;
}

namespace Gfx
{
  class Canvas;
}

/// XBmapRenderer is "in between jobs"... needs to be merged into GLCanvas.
namespace XBmapRenderer
{
   void doRender(Display* display,
                 Window win,
                 int screen,
                 Gfx::Canvas& canvas,
                 const media::bmap_data& data,
                 const geom::vec2<double>& world_pos,
                 const geom::vec2<double>& zoom);
};

//---------------------------------------------------------------------
//
// Implementation
//
//---------------------------------------------------------------------

#include "geom/vec2.h"

#include "gfx/canvas.h"

#include "media/bmapdata.h"

#include "rutz/error.h"

#include <GL/glx.h>

#include "rutz/trace.h"
#include "rutz/debug.h"
DBG_REGISTER

void XBmapRenderer::doRender(Display* display,
                             Window win,
                             int screen,
                             Gfx::Canvas& canvas,
                             const media::bmap_data& data,
                             const geom::vec2<double>& world_pos,
                             const geom::vec2<double>& /* zoom */)
{
DOTRACE("XBmapRenderer::doRender");

  XWindowAttributes xwa;
  Status status = XGetWindowAttributes(display, win, &xwa);
  if (status == 0)
    {
      throw rutz::error("couldn't get X11 window attributes", SRC_POS);
    }
  Visual* visual = xwa.visual;

  int format = (data.bits_per_pixel() == 1) ? XYBitmap : ZPixmap;

  data.set_row_order(media::bmap_data::TOP_FIRST);

  // Calculate GL window coordinates of lower left corner of image
  geom::vec2i screen_pos = geom::vec2i(canvas.screenFromWorld2(world_pos));

  dbg_dump(3, world_pos);
  dbg_dump(3, screen_pos);

  // Calculate GL window coordinates for upper left corner of image
  screen_pos.y() += data.height();

  // Calculate X11 window coordinates for upper left corner of image
  // (X11 coordinates put 0 at the top of the window, while GL
  // coordinates put 0 at the bottom of the window)
  screen_pos.y() = xwa.height - screen_pos.y();

  // Create the XImage
  XImage* image = XCreateImage(display, visual,
                               data.bits_per_pixel(), /* bit depth */
                               format, /* format = XYBitmap, XYPixmap, ZPixmap */
                               0, /* offset */
                               reinterpret_cast<char*>(data.bytes_ptr()),
                               data.width(), data.height(),
                               data.byte_alignment()*8, /* bitmap_pad */
                               0); /* bytes_per_line */

  if (image == NULL)
    {
      throw rutz::error("couldn't create an XImage", SRC_POS);
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

static const char vcid_xbmaprenderer_h[] = "$Id$ $URL$";
#endif // !XBMAPRENDERER_H_DEFINED
