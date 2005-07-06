/** @file tk/tclpkg-widget.cc tcl interface package for tcl::tkWidget class */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2003-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Wed May 28 14:45:26 2003
// commit: $Id$
// $HeadURL$
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

#ifndef GROOVX_TK_TCLPKG_WIDGET_CC_UTC20050628165845_DEFINED
#define GROOVX_TK_TCLPKG_WIDGET_CC_UTC20050628165845_DEFINED

#include "tk/tclpkg-widget.h"

#include "tcl/objpkg.h"
#include "tcl/pkg.h"
#include "tk/widget.h"

#include "rutz/fstring.h"

#include "rutz/trace.h"

extern "C"
int Tkwidget_Init(Tcl_Interp* interp)
{
GVX_TRACE("Tkwidget_Init");

  GVX_PKG_CREATE(pkg, interp, "TkWidget", "4.$Revision$");

  pkg->inherit_pkg("Obj");
  tcl::def_basic_type_cmds<tcl::TkWidget>(pkg, SRC_POS);

  pkg->def( "bind", "event_sequence binding_script", &tcl::TkWidget::bind, SRC_POS );

  pkg->def_get_set("cursor", &tcl::TkWidget::getCursor, &tcl::TkWidget::setCursor, SRC_POS);
  pkg->def_action("destroy", &tcl::TkWidget::destroyWidget, SRC_POS);
  pkg->def_action("grabKeyboard", &tcl::TkWidget::grabKeyboard, SRC_POS);
  pkg->def_get_set("height", &tcl::TkWidget::height, &tcl::TkWidget::setHeight, SRC_POS);
  pkg->def_action("loseFocus", &tcl::TkWidget::loseFocus, SRC_POS);
  pkg->def_action("maximize", &tcl::TkWidget::maximize, SRC_POS);
  pkg->def_action("minimize", &tcl::TkWidget::minimize, SRC_POS);
  pkg->def_getter("pathname", &tcl::TkWidget::pathname, SRC_POS);
  pkg->def_getter("pixelsPerInch", &tcl::TkWidget::pixelsPerInch, SRC_POS);
  pkg->def_action("unpack", &tcl::TkWidget::unpack, SRC_POS);
  pkg->def("repack", "pack_args", &tcl::TkWidget::repack, SRC_POS);
  pkg->def_action("takeFocus", &tcl::TkWidget::takeFocus, SRC_POS);
  pkg->def("warpPointer", "objref x y", &tcl::TkWidget::warpPointer, SRC_POS);
  pkg->def_get_set("width", &tcl::TkWidget::width, &tcl::TkWidget::setWidth, SRC_POS);
  pkg->def_action("winInfo", &tcl::TkWidget::winInfo, SRC_POS);

  pkg->def_action("hook", &tcl::TkWidget::hook, SRC_POS);

  GVX_PKG_RETURN(pkg);
}

static const char vcid_groovx_tk_tclpkg_widget_cc_utc20050628165845[] = "$Id$ $HeadURL$";
#endif // !GROOVX_TK_TCLPKG_WIDGET_CC_UTC20050628165845_DEFINED
