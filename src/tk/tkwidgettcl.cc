///////////////////////////////////////////////////////////////////////
//
// tkwidgettcl.cc
//
// Copyright (c) 2003-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Wed May 28 14:45:26 2003
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

#ifndef TKWIDGETTCL_CC_DEFINED
#define TKWIDGETTCL_CC_DEFINED

#include "tcl/objpkg.h"
#include "tcl/tclpkg.h"
#include "tk/tkwidget.h"

#include "util/fstring.h"

#include "util/trace.h"

extern "C"
int Tkwidget_Init(Tcl_Interp* interp)
{
DOTRACE("Tkwidget_Init");

  PKG_CREATE(interp, "TkWidget", "4.$Revision$");

  pkg->inheritPkg("Obj");
  Tcl::defGenericObjCmds<Tcl::TkWidget>(pkg, SRC_POS);

  pkg->def( "bind", "event_sequence binding_script", &Tcl::TkWidget::bind, SRC_POS );

  pkg->defAttrib("cursor", &Tcl::TkWidget::getCursor, &Tcl::TkWidget::setCursor, SRC_POS);
  pkg->defAction("destroy", &Tcl::TkWidget::destroyWidget, SRC_POS);
  pkg->defAction("grabKeyboard", &Tcl::TkWidget::grabKeyboard, SRC_POS);
  pkg->defAttrib("height", &Tcl::TkWidget::height, &Tcl::TkWidget::setHeight, SRC_POS);
  pkg->defAction("loseFocus", &Tcl::TkWidget::loseFocus, SRC_POS);
  pkg->defAction("maximize", &Tcl::TkWidget::maximize, SRC_POS);
  pkg->defAction("minimize", &Tcl::TkWidget::minimize, SRC_POS);
  pkg->defGetter("pathname", &Tcl::TkWidget::pathname, SRC_POS);
  pkg->defGetter("pixelsPerInch", &Tcl::TkWidget::pixelsPerInch, SRC_POS);
  pkg->defAction("unpack", &Tcl::TkWidget::unpack, SRC_POS);
  pkg->def("repack", "pack_args", &Tcl::TkWidget::repack, SRC_POS);
  pkg->defAction("takeFocus", &Tcl::TkWidget::takeFocus, SRC_POS);
  pkg->def("warpPointer", "objref x y", &Tcl::TkWidget::warpPointer, SRC_POS);
  pkg->defAttrib("width", &Tcl::TkWidget::width, &Tcl::TkWidget::setWidth, SRC_POS);
  pkg->defAction("winInfo", &Tcl::TkWidget::winInfo, SRC_POS);

  pkg->defAction("hook", &Tcl::TkWidget::hook, SRC_POS);

  PKG_RETURN;
}

static const char vcid_tkwidgettcl_cc[] = "$Id$ $URL$";
#endif // !TKWIDGETTCL_CC_DEFINED
