/** @file visx/tclpkg-fixpt.cc tcl interface package for class FixPt */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Mon Jan  4 08:00:00 1999
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

#include "visx/tclpkg-fixpt.h"

#include "tcl-io/fieldpkg.h"

#include "visx/fixpt.h"

#include "rutz/trace.h"

extern "C"
int Fixpt_Init(Tcl_Interp* interp)
{
GVX_TRACE("Fixpt_Init");

  return tcl::pkg::init
    (interp, "FixPt", "4.0",
     [](tcl::pkg* pkg) {
      pkg->inherit_pkg("GxShapeKit");
      tcl::defFieldContainer<FixPt>(pkg, SRC_POS);
      tcl::def_creator<FixPt>(pkg);
    });
}
