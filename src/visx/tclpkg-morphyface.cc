/** @file visx/tclpkg-morphyface.cc tcl interface package for class
    MorphyFace */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Wed Sep  8 15:42:36 1999
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

#include "visx/tclpkg-morphyface.h"

#include "tcl-io/fieldpkg.h"

#include "visx/morphyface.h"

#include "rutz/trace.h"

extern "C"
int Morphyface_Init(Tcl_Interp* interp)
{
GVX_TRACE("Morphyface_Init");

  return tcl::pkg::init
    (interp, "MorphyFace", "4.0",
     [](tcl::pkg* pkg) {
      pkg->inherit_pkg("GxShapeKit");
      tcl::defFieldContainer<MorphyFace>(pkg, SRC_POS);
      tcl::def_creator<MorphyFace>(pkg);
    });
}
