///////////////////////////////////////////////////////////////////////
//
// toglettcl.cc
//
// Copyright (c) 1998-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Mon Nov  2 08:00:00 1998 (as objtogl.cc)
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
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
  Tcl::List dumpCmap(Util::SoftRef<Toglet> toglet,
                     unsigned int start, unsigned end)
  {
    if (start > 255 || end > 255)
      {
        throw Util::Error("colormap index out of range");
      }

    Tcl::List result;

    for (unsigned int i = start; i <= end; ++i)
      {
        Toglet::Color color = toglet->queryColor(i);
        Tcl::List color_list;
        color_list.append(i);
        color_list.append(color.red);
        color_list.append(color.green);
        color_list.append(color.blue);
        result.append(color_list);
      }

    return result;
  }

  Tcl::List dumpCmapAll(Util::SoftRef<Toglet> toglet)
  {
    return dumpCmap(toglet, 0, 255);
  }

  // Make a specified GxNode the widget's current drawable, and draw
  // it in the OpenGL window. The widget's visibility is set to true.
  Util::UID see(Util::SoftRef<Toglet> widg, Util::Ref<GxNode> item)
  {
    widg->setDrawable(item);
    widg->setVisibility(true);
    widg->fullRender();
    return item->id();
  }
}

extern "C"
int Toglet_Init(Tcl_Interp* interp)
{
DOTRACE("Toglet_Init");

  Tcl::Pkg* pkg = new Tcl::Pkg(interp, "Toglet", "$Revision$");

  pkg->inheritPkg("TkWidget");
  Tcl::defGenericObjCmds<Toglet>(pkg);

  Util::ObjFactory::theOne().registerCreatorFunc( &Toglet::make );
  Util::ObjFactory::theOne().registerCreatorFunc( &Toglet::makeToplevel,
                                                  "TopToglet" );

  pkg->def( "current", "toglet_id", &Toglet::setCurrent );
  pkg->def( "current", 0, &Toglet::getCurrent );
  pkg->def( "defaultParent", "parent", &Toglet::defaultParent );
  pkg->def( "dumpCmap", "toglet_id start_index end_index", &dumpCmap );
  pkg->def( "dumpCmap", "toglet_id", &dumpCmapAll );
  pkg->def( "see", "gxnode_id", &see );

  pkg->defSetter("allowRefresh", &Toglet::allowRefresh);
  pkg->defSetter("animate", "item_id(s) frames_per_second", &Toglet::animate);
  pkg->defAttrib("camera", &Toglet::getCamera, &Toglet::setCamera);
  pkg->defAction("clearscreen", &Toglet::fullClearscreen);
  pkg->defSetter("hold", "item_id(s) hold_on?", &Toglet::setHold);
  pkg->defSetter("setVisible", "item_id(s) visibility", &Toglet::setVisibility);
  pkg->defAttrib("size", &Toglet::size, &Toglet::setSize);
  pkg->defAction("swapBuffers", &Toglet::swapBuffers);
  pkg->defAction("takeFocus", &Toglet::takeFocus);
  pkg->defAction("undraw", &Toglet::undraw);

  Toglet::make();

  pkg->eval("proc clearscreen {} { Togl::clearscreen }\n"
            "proc see {id} { Togl::see $id }\n"
            "proc undraw {} { Togl::undraw }\n"
            "\n"
            "foreach cmd [info commands ::Toglet::*] {\n"
            "  namespace eval ::Togl {\n"
            "    proc [namespace tail $cmd] {args} \" eval $cmd \\[Toglet::current\\] \\$args \""
            "  }\n"
            "}\n"
            "namespace eval ::Togl { namespace export * }"
            );

  return pkg->initStatus();
}

static const char vcid_toglettcl_cc[] = "$Header$";
#endif // !TOGLETTCL_CC_DEFINED
