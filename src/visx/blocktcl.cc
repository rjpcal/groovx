///////////////////////////////////////////////////////////////////////
//
// blocktcl.cc
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Wed Jun 16 19:46:54 1999
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

#ifndef BLOCKTCL_CC_DEFINED
#define BLOCKTCL_CC_DEFINED

#include "tcl/objpkg.h"
#include "tcl/tclpkg.h"
#include "tcl/tracertcl.h"

#include "visx/block.h"

#include "util/trace.h"

extern "C"
int Block_Init(Tcl_Interp* interp)
{
DOTRACE("Block_Init");

  PKG_CREATE(interp, "Block", "4.$Revision$");
  pkg->inheritPkg("ElementContainer");
  Tcl::defGenericObjCmds<Block>(pkg, SRC_POS);
  Tcl::defCreator<Block>(pkg);

  Tcl::defTracing(pkg, Block::tracer);

  PKG_RETURN;
}

static const char vcid_blocktcl_cc[] = "$Id$ $URL$";
#endif // !BLOCKTCL_CC_DEFINED
