/** @file visx/tclpkg-block.cc tcl interface package for class Block */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Wed Jun 16 19:46:54 1999
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

#ifndef GROOVX_VISX_TCLPKG_BLOCK_CC_UTC20050628171008_DEFINED
#define GROOVX_VISX_TCLPKG_BLOCK_CC_UTC20050628171008_DEFINED

#include "visx/tclpkg-block.h"

#include "visx/tclpkg-block.h"

#include "tcl/objpkg.h"
#include "tcl/pkg.h"
#include "tcl/tracertcl.h"

#include "visx/block.h"

#include "rutz/trace.h"

extern "C"
int Block_Init(Tcl_Interp* interp)
{
GVX_TRACE("Block_Init");

  GVX_PKG_CREATE(pkg, interp, "Block", "4.$Revision$");
  pkg->inherit_pkg("ElementContainer");
  tcl::def_basic_type_cmds<Block>(pkg, SRC_POS);
  tcl::def_creator<Block>(pkg);

  tcl::def_tracing(pkg, Block::tracer);

  GVX_PKG_RETURN(pkg);
}

static const char __attribute__((used)) vcid_groovx_visx_tclpkg_block_cc_utc20050628171008[] = "$Id$ $HeadURL$";
#endif // !GROOVX_VISX_TCLPKG_BLOCK_CC_UTC20050628171008_DEFINED
