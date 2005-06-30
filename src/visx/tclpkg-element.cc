///////////////////////////////////////////////////////////////////////
//
// elementtcl.cc
//
// Copyright (c) 2003-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Tue May 27 15:38:33 2003
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

#ifndef GROOVX_VISX_TCLPKG_ELEMENT_CC_UTC20050628171008_DEFINED
#define GROOVX_VISX_TCLPKG_ELEMENT_CC_UTC20050628171008_DEFINED

#include "visx/tclpkg-element.h"

#include "visx/tclpkg-element.h"

#include "gfx/toglet.h"

#include "tcl/objpkg.h"
#include "tcl/pkg.h"

#include "rutz/fstring.h"

#include "visx/element.h"

#include "rutz/trace.h"

extern "C"
int Element_Init(Tcl_Interp* interp)
{
GVX_TRACE("Element_Init");

  GVX_PKG_CREATE(pkg, interp, "Element", "4.$Revision$");
  pkg->inherit_pkg("IO");
  tcl::def_basic_type_cmds<Element>(pkg, SRC_POS);

  pkg->def_getter("widget", &Element::getWidget, SRC_POS);
  pkg->def_getter("trialType", &Element::trialType, SRC_POS);
  pkg->def_getter("lastResponse", &Element::lastResponse, SRC_POS);
  pkg->def_getter("info", &Element::vxInfo, SRC_POS);
  pkg->def_action("halt", &Element::vxHalt, SRC_POS);
  pkg->def_action("undo", &Element::vxUndo, SRC_POS);
  pkg->def_action("reset", &Element::vxReset, SRC_POS);

  GVX_PKG_RETURN(pkg);
}

static const char vcid_groovx_visx_tclpkg_element_cc_utc20050628171008[] = "$Id$ $HeadURL$";
#endif // !GROOVX_VISX_TCLPKG_ELEMENT_CC_UTC20050628171008_DEFINED
