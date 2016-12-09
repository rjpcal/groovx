/** @file visx/tclpkg-element.cc tcl interface package for class Element */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2003-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Tue May 27 15:38:33 2003
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

#include "visx/tclpkg-element.h"

#include "tcl/objpkg.h"
#include "tcl/pkg.h"

#include "tcl-gfx/toglet.h"

#include "rutz/fstring.h"

#include "visx/element.h"

#include "rutz/trace.h"

extern "C"
int Element_Init(Tcl_Interp* interp)
{
GVX_TRACE("Element_Init");

  return tcl::pkg::init
    (interp, "Element", "4.0",
     [](tcl::pkg* pkg) {
      pkg->inherit_pkg("io");
      tcl::def_basic_type_cmds<Element>(pkg, SRC_POS);

      pkg->def_getter("widget", &Element::getWidget, SRC_POS);
      pkg->def_getter("trialType", &Element::trialType, SRC_POS);
      pkg->def_getter("lastResponse", &Element::lastResponse, SRC_POS);
      pkg->def_getter("info", &Element::vxInfo, SRC_POS);
      pkg->def_action("halt", &Element::vxHalt, SRC_POS);
      pkg->def_action("undo", &Element::vxUndo, SRC_POS);
      pkg->def_action("reset", &Element::vxReset, SRC_POS);
    });
}
