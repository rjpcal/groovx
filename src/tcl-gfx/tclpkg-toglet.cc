/** @file gfx/tclpkg-toglet.cc tcl interface package for Toglet */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1998-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Mon Nov  2 08:00:00 1998 (as objtogl.cc)
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
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

#include "tcl-gfx/tclpkg-toglet.h"

#include "gfx/glcanvas.h"
#include "gfx/gxcamera.h"
#include "gfx/gxnode.h"

#include "nub/objfactory.h"
#include "nub/ref.h"

#include "tcl/objpkg.h"
#include "tcl/list.h"
#include "tcl/pkg.h"

#include "tcl-gfx/toglet.h"
#include "tcl-gfx/vectcl.h"

#include "rutz/error.h"

#include "rutz/trace.h"

namespace
{
  // Make a specified GxNode the widget's current drawable, and draw
  // it in the OpenGL window. The widget's visibility is set to true.
  nub::uid see(nub::soft_ref<Toglet> widg, nub::ref<GxNode> item)
  {
    widg->setDrawable(item);
    widg->setVisibility(true);
    widg->fullRender();
    return item->id();
  }

  // We need to eagerly drop references to objects at shutdown time,
  // because Tcl tends to prematurely unload dynamically-loaded
  // packages... so we need to make sure we don't have any references
  // to objects whose members are defined in such packages.
  void clear_on_exit()
  {
    if (Toglet::getCurrent().is_valid())
      {
        Toglet::getCurrent()->setVisibility(false);
      }
  }
}

#include <tcl.h>

extern "C"
int Toglet_Init(Tcl_Interp* interp)
{
GVX_TRACE("Toglet_Init");

  if (Tcl_PkgRequire(interp, "Tk", 0, 0) == 0)
    return 1;

  return tcl::pkg::init
    (interp, "Toglet", "4.$Revision$",
     [](tcl::pkg* pkg) {

      pkg->on_exit( &clear_on_exit );

      pkg->inherit_pkg("TkWidget");
      tcl::def_basic_type_cmds<Toglet>(pkg, SRC_POS);

      nub::obj_factory::instance().register_creator( &Toglet::make );
      nub::obj_factory::instance().register_creator( &Toglet::makeToplevel,
                                                     "TopToglet" );

      pkg->def( "::tog", 0, &Toglet::getCurrent, SRC_POS );
      pkg->def( "::cv", 0, &GLCanvas::getCurrent, SRC_POS );

      pkg->def( "current", "toglet_id", &Toglet::setCurrent, SRC_POS );
      pkg->def( "current", 0, &Toglet::getCurrent, SRC_POS );
      pkg->def( "defaultParent", "parent", &Toglet::defaultParent, SRC_POS );
      pkg->def( "see", "gxnode_id", &see, SRC_POS );

      pkg->def_setter("allowRefresh", &Toglet::allowRefresh, SRC_POS);
      pkg->def("animate", "objref(s) frames_per_second", &Toglet::animate, SRC_POS);
      pkg->def_get_set("camera", &Toglet::getCamera, &Toglet::setCamera, SRC_POS);
      pkg->def_getter("canvas", &Toglet::getCanvas, SRC_POS);
      pkg->def_action("clearscreen", &Toglet::fullClearscreen, SRC_POS);
      pkg->def("hold", "objref(s) hold_on", &Toglet::setHold, SRC_POS);
      pkg->def("setVisible", "objref(s) visibility", &Toglet::setVisibility, SRC_POS);
      pkg->def_get_set("size", &Toglet::size, &Toglet::setSize, SRC_POS);
      pkg->def_action("swapBuffers", &Toglet::swapBuffers, SRC_POS);
      pkg->def_action("undraw", &Toglet::undraw, SRC_POS);

      Toglet::make();

      pkg->eval("proc ::clearscreen {} { Toglet::clearscreen [Toglet::current] }\n"
                "proc ::see {id} { Toglet::see [Toglet::current] $id }\n"
                "proc ::undraw {} { Toglet::undraw [Toglet::current] }\n"
                );
    });
}
