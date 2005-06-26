///////////////////////////////////////////////////////////////////////
//
// toglettcl.cc
//
// Copyright (c) 1998-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Mon Nov  2 08:00:00 1998 (as objtogl.cc)
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

#ifndef GROOVX_GFX_TOGLETTCL_CC_UTC20050626084023_DEFINED
#define GROOVX_GFX_TOGLETTCL_CC_UTC20050626084023_DEFINED

#include "gfx/toglettcl.h"

#include "gfx/canvas.h"
#include "gfx/gxcamera.h"
#include "gfx/gxnode.h"
#include "gfx/toglet.h"
#include "gfx/vectcl.h"

#include "nub/objfactory.h"
#include "nub/ref.h"

#include "tcl/objpkg.h"
#include "tcl/tcllistobj.h"
#include "tcl/tclpkg.h"

#include "rutz/error.h"

#include "rutz/trace.h"

namespace
{
  // Make a specified GxNode the widget's current drawable, and draw
  // it in the OpenGL window. The widget's visibility is set to true.
  Nub::UID see(Nub::SoftRef<Toglet> widg, Nub::Ref<GxNode> item)
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
  void clearOnExit()
  {
    if (Toglet::getCurrent().isValid())
      {
        Toglet::getCurrent()->setVisibility(false);
      }
  }

  Nub::SoftRef<Gfx::Canvas> currentCanvas()
  {
    return Toglet::getCurrent()->getCanvas();
  }
}

#include <tcl.h>

extern "C"
int Toglet_Init(Tcl_Interp* interp)
{
DOTRACE("Toglet_Init");

  if (Tcl_PkgRequire(interp, "Tk", 0, 0) == 0)
    return 1;

  PKG_CREATE(interp, "Toglet", "4.$Revision$");

  pkg->onExit( &clearOnExit );

  pkg->inheritPkg("TkWidget");
  Tcl::defGenericObjCmds<Toglet>(pkg, SRC_POS);

  Nub::ObjFactory::theOne().register_creator( &Toglet::make );
  Nub::ObjFactory::theOne().register_creator( &Toglet::makeToplevel,
                                              "TopToglet" );

  pkg->def( "::tog", 0, &Toglet::getCurrent, SRC_POS );
  pkg->def( "::cv", 0, &currentCanvas, SRC_POS );

  pkg->def( "current", "toglet_id", &Toglet::setCurrent, SRC_POS );
  pkg->def( "current", 0, &Toglet::getCurrent, SRC_POS );
  pkg->def( "defaultParent", "parent", &Toglet::defaultParent, SRC_POS );
  pkg->def( "see", "gxnode_id", &see, SRC_POS );

  pkg->defSetter("allowRefresh", &Toglet::allowRefresh, SRC_POS);
  pkg->def("animate", "objref(s) frames_per_second", &Toglet::animate, SRC_POS);
  pkg->defAttrib("camera", &Toglet::getCamera, &Toglet::setCamera, SRC_POS);
  pkg->defGetter("canvas", &Toglet::getCanvas, SRC_POS);
  pkg->defAction("clearscreen", &Toglet::fullClearscreen, SRC_POS);
  pkg->def("hold", "objref(s) hold_on", &Toglet::setHold, SRC_POS);
  pkg->def("setVisible", "objref(s) visibility", &Toglet::setVisibility, SRC_POS);
  pkg->defAttrib("size", &Toglet::size, &Toglet::setSize, SRC_POS);
  pkg->defAction("swapBuffers", &Toglet::swapBuffers, SRC_POS);
  pkg->defAction("undraw", &Toglet::undraw, SRC_POS);

  Toglet::make();

  pkg->eval("proc ::clearscreen {} { Toglet::clearscreen [Toglet::current] }\n"
            "proc ::see {id} { Toglet::see [Toglet::current] $id }\n"
            "proc ::undraw {} { Toglet::undraw [Toglet::current] }\n"
            );

  PKG_RETURN;
}

static const char vcid_groovx_gfx_toglettcl_cc_utc20050626084023[] = "$Id$ $HeadURL$";
#endif // !GROOVX_GFX_TOGLETTCL_CC_UTC20050626084023_DEFINED
