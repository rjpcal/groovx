/** @file gfx/tclpkg-canvas.cc tcl interface packages for Gfx::Canvas
    and derivatives */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Sat Oct 23 19:24:11 2004
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [https://github.com/rjpcal/groovx]
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

#include "tcl-gfx/tclpkg-canvas.h"

#include "gfx/canvas.h"
#include "gfx/glcanvas.h"

#include "media/bmapdata.h"

#include "tcl/objpkg.h"
#include "tcl/pkg.h"

#include "tcl-gfx/recttcl.h"
#include "tcl-gfx/vectcl.h"

#include "rutz/trace.h"

namespace
{
  //--------------------------------------------------------------------
  //
  // pixelCheckSum --
  //
  // This command returns the sum of the color indices of all the pixels
  // in a specified rectangle. It can be used as an easy way to see if
  // something is present within a given rectangle on the screen. For
  // example, if the background is index 0, and the foreground is index
  // 1, than this command can tell how many pixels were drawn by a
  // particular drawing command. However-- it is not at all speedy, so
  // it is best used for testing only.
  //
  //--------------------------------------------------------------------

  long int pixelCheckSum(nub::soft_ref<GLCanvas> canvas,
                         int x, int y, int w, int h)
  {
    const geom::rect<int> bounds = geom::rect<int>().set_lbwh(x, y, w, h);

    media::bmap_data data = canvas->grabPixels(bounds);

    return data.bytes_sum();
  }

  long int pixelCheckSumAll(nub::soft_ref<GLCanvas> canvas)
  {
    const geom::rect<int> viewport = canvas->getScreenViewport();
    return pixelCheckSum(canvas,
                         viewport.left(),
                         viewport.bottom(),
                         viewport.width(),
                         viewport.height());
  }

  geom::vec3d topLeft(nub::soft_ref<GLCanvas> canvas)
  {
    const geom::recti vp = canvas->getScreenViewport();
    const geom::vec2i tl = vp.top_left();
    return canvas->worldFromScreen3(geom::vec3d(tl.x(), tl.y(), 0.5));
  }
}

extern "C"
int Canvas_Init(Tcl_Interp* interp)
{
GVX_TRACE("Canvas_Init");

  return tcl::pkg::init
    (interp, "Canvas", "4.0",
     [](tcl::pkg* pkg) {
      pkg->inherit_pkg("Obj");
      tcl::def_basic_type_cmds<Gfx::Canvas>(pkg, SRC_POS);

      pkg->def_getter("viewport", &Gfx::Canvas::getScreenViewport, SRC_POS);
      pkg->def("topLeft", "canvas", &topLeft, SRC_POS);

      pkg->def("throwIfError", "",
               [](nub::ref<Gfx::Canvas> c){c->throwIfError("", SRC_POS);}, SRC_POS);
    });
}

extern "C"
int Glcanvas_Init(Tcl_Interp* interp)
{
GVX_TRACE("Glcanvas_Init");

  return tcl::pkg::init
    (interp, "GLCanvas", "4.0",
     [](tcl::pkg* pkg) {
      pkg->inherit_pkg("Canvas");
      tcl::def_basic_type_cmds<GLCanvas>(pkg, SRC_POS);

      pkg->def( "pixelCheckSum", "glcanvas x y w h", &pixelCheckSum, SRC_POS );
      pkg->def( "pixelCheckSum", "glcanvas", &pixelCheckSumAll, SRC_POS );
    });
}
