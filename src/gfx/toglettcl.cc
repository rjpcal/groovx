///////////////////////////////////////////////////////////////////////
//
// toglettcl.cc
//
// Copyright (c) 1998-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Mon Nov  2 08:00:00 1998 (as objtogl.cc)
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

#ifndef TOGLETTCL_CC_DEFINED
#define TOGLETTCL_CC_DEFINED

#include "gfx/gxcamera.h"
#include "gfx/gxnode.h"
#include "gfx/pointtcl.h"
#include "gfx/toglet.h"

#include "tcl/objpkg.h"
#include "tcl/tcllistobj.h"
#include "tcl/tclpkg.h"

#include "util/error.h"
#include "util/objfactory.h"
#include "util/ref.h"

#include "util/trace.h"

namespace
{
  // Make a specified GxNode the widget's current drawable, and draw
  // it in the OpenGL window. The widget's visibility is set to true.
  Util::UID see(Util::SoftRef<Toglet> widg, Util::Ref<GxNode> item)
  {
    widg->setDrawable(item);
    widg->setVisibility(true);
    widg->fullRender();
    return item->id();
  }

  // We need to eagerly drop references to objects at shutdown time,
  // because Tcl tends to prematurely unload dynamically-loaded
  // packages... so we need to make sure we don't have any references to
  // objects whose members are defined in such packages.
  void clearOnExit()
  {
    if (Toglet::getCurrent().isValid())
      {
        Toglet::getCurrent()->setVisibility(false);
      }
  }
}

extern "C"
int Toglet_Init(Tcl_Interp* interp)
{
DOTRACE("Toglet_Init");

  PKG_CREATE(interp, "Toglet", "$Revision$");

  pkg->onExit( &clearOnExit );

  pkg->inheritPkg("TkWidget");
  Tcl::defGenericObjCmds<Toglet>(pkg, SRC_POS);

  Util::ObjFactory::theOne().registerCreatorFunc( &Toglet::make );
  Util::ObjFactory::theOne().registerCreatorFunc( &Toglet::makeToplevel,
                                                  "TopToglet" );

  pkg->def( "current", "toglet_id", &Toglet::setCurrent, SRC_POS );
  pkg->def( "current", 0, &Toglet::getCurrent, SRC_POS );
  pkg->def( "defaultParent", "parent", &Toglet::defaultParent, SRC_POS );
  pkg->def( "see", "gxnode_id", &see, SRC_POS );

  pkg->defSetter("allowRefresh", &Toglet::allowRefresh, SRC_POS);
  pkg->defSetter("animate", "objref(s) frames_per_second", &Toglet::animate, SRC_POS);
  pkg->defAttrib("camera", &Toglet::getCamera, &Toglet::setCamera, SRC_POS);
  pkg->defAction("clearscreen", &Toglet::fullClearscreen, SRC_POS);
  pkg->defSetter("hold", "objref(s) hold_on?", &Toglet::setHold, SRC_POS);
  pkg->defSetter("setVisible", "objref(s) visibility", &Toglet::setVisibility, SRC_POS);
  pkg->defAttrib("size", &Toglet::size, &Toglet::setSize, SRC_POS);
  pkg->defAction("swapBuffers", &Toglet::swapBuffers, SRC_POS);
  pkg->defAction("takeFocus", &Toglet::takeFocus, SRC_POS);
  pkg->defAction("undraw", &Toglet::undraw, SRC_POS);

  Toglet::make();

  pkg->eval("proc ::clearscreen {} { Toglet::clearscreen [Toglet::current] }\n"
            "proc ::see {id} { Toglet::see [Toglet::current] $id }\n"
            "proc ::undraw {} { Toglet::undraw [Toglet::current] }\n"
            );

  PKG_RETURN;
}

static const char vcid_toglettcl_cc[] = "$Header$";
#endif // !TOGLETTCL_CC_DEFINED
