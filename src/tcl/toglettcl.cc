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

#include "tcl/objpkg.h"
#include "tcl/tclerror.h"
#include "tcl/tcllistobj.h"
#include "tcl/tclpkg.h"
#include "tcl/toglet.h"

#include "util/objfactory.h"
#include "util/ref.h"

#include "util/trace.h"
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// TogletTcl namespace declarations
//
///////////////////////////////////////////////////////////////////////

namespace
{
  Tcl::List dumpCmap(SoftRef<Toglet> toglet, unsigned int start, unsigned end)
  {
    if (start > 255 || end > 255)
      {
        throw Tcl::TclError("colormap index out of range");
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

  Tcl::List dumpCmapAll(SoftRef<Toglet> toglet)
  {
    return dumpCmap(toglet, 0, 255);
  }

  bool inited()
  {
    return Toglet::getCurrent().isValid();
  }

  // Make a specified GxNode the widget's current drawable, and draw
  // it in the OpenGL window. The widget's visibility is set to true.
  Util::UID see(SoftRef<Toglet> widg, Ref<GxNode> item)
  {
    widg->setDrawable(item);
    widg->setVisibility(true);
    widg->fullRender();
    return item->id();
  }
}

//---------------------------------------------------------------------
//
// TogletPkg class definition
//
//---------------------------------------------------------------------

class TogletPkg : public Tcl::Pkg
{
public:
  TogletPkg(Tcl_Interp* interp) :
    Tcl::Pkg(interp, "Toglet", "$Revision$")
  {
    this->inheritPkg("Obj");
    Tcl::defGenericObjCmds<Toglet>(this);

    def( "bind", "event_sequence binding_script", &Toglet::bind );
    def( "current", "toglet_id", &Toglet::setCurrent );
    def( "current", 0, &Toglet::getCurrent );
    def( "defaultParent", "parent", &Toglet::defaultParent );
    def( "dumpCmap", "toglet_id start_index end_index", &dumpCmap );
    def( "dumpCmap", "toglet_id", &dumpCmapAll );
    def( "inited", 0, &inited );
    def( "see", "gxnode_id", &see );

    defSetter("allowRefresh", &Toglet::allowRefresh);
    defSetter("animate", "item_id(s) frames_per_second", &Toglet::animate);
    defAttrib("camera", &Toglet::getCamera, &Toglet::setCamera);
    defAction("clearscreen", &Toglet::fullClearscreen);
    defAction("destroy", &Tcl::TkWidget::destroyWidget);
    defAction("grabKeyboard", &Tcl::TkWidget::grabKeyboard);
    defAttrib("height", &Tcl::TkWidget::height, &Tcl::TkWidget::setHeight);
    defSetter("hold", "item_id(s) hold_on?", &Toglet::setHold);
    defAction("maximize", &Tcl::TkWidget::maximize);
    defAction("minimize", &Tcl::TkWidget::minimize);
    defGetter("pathname", &Tcl::TkWidget::pathname);
    defGetter("pixelsPerInch", &Tcl::TkWidget::pixelsPerInch);
    defSetter("setVisible", "item_id(s) visibility", &Toglet::setVisibility);
    defAttrib("size", &Toglet::size, &Toglet::setSize);
    defAction("swapBuffers", &Toglet::swapBuffers);
    defAction("takeFocus", &Toglet::takeFocus);
    defAction("undraw", &Toglet::undraw);
    defAttrib("width", &Tcl::TkWidget::width, &Tcl::TkWidget::setWidth);

    defAction("hook", &Tcl::TkWidget::hook);

    Toglet::make();

    Pkg::eval("proc clearscreen {} { Togl::clearscreen }\n"
              "proc see {id} { Togl::see $id }\n"
              "proc undraw {} { Togl::undraw }\n");

    Pkg::eval("foreach cmd [info commands ::Toglet::*] {\n"
              "  namespace eval ::Togl {\n"
              "    proc [namespace tail $cmd] {args} \" eval $cmd \\[Toglet::current\\] \\$args \""
              "  }\n"
              "}\n"
              "namespace eval ::Togl { namespace export * }"
              );

    Pkg::eval("Expt::widget [Toglet::current]");
  }

  virtual ~TogletPkg()
  {
    if (Toglet::getCurrent().isValid())
      {
        Toglet::getCurrent()->setVisibility(false);
      }
  }
};

//---------------------------------------------------------------------
//
// Toglet_Init
//
//---------------------------------------------------------------------

extern "C"
int Toglet_Init(Tcl_Interp* interp)
{
DOTRACE("Toglet_Init");

  Tcl::Pkg* pkg = new TogletPkg(interp);

  Util::ObjFactory::theOne().registerCreatorFunc( &Toglet::make );
  Util::ObjFactory::theOne().registerCreatorFunc( &Toglet::makeToplevel,
                                                  "TopToglet" );

  return pkg->initStatus();
}

static const char vcid_toglettcl_cc[] = "$Header$";
#endif // !TOGLETTCL_CC_DEFINED
