/** @file visx/tclpkg-house.cc tcl interface package for class House */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Mon Sep 13 15:14:19 1999
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

#include "visx/tclpkg-house.h"

#include "tcl-io/fieldpkg.h"

#include "visx/house.h"

#include "rutz/trace.h"


extern "C"
int House_Init(Tcl_Interp* interp)
{
GVX_TRACE("House_Init");

  return tcl::pkg::init
    (interp, "House", "4.0",
     [](tcl::pkg* pkg) {
      pkg->inherit_pkg("GxShapeKit");
      tcl::defFieldContainer<House>(pkg, SRC_POS);
      tcl::def_creator<House>(pkg);
    });
}
