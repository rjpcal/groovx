///////////////////////////////////////////////////////////////////////
//
// housetcl.cc
//
// Copyright (c) 1999-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Mon Sep 13 15:14:19 1999
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

#ifndef HOUSETCL_CC_DEFINED
#define HOUSETCL_CC_DEFINED

#include "io/fieldpkg.h"

#include "visx/house.h"

#include "util/trace.h"


extern "C"
int House_Init(Tcl_Interp* interp)
{
DOTRACE("House_Init");

  PKG_CREATE(interp, "House", "4.$Revision$");
  pkg->inheritPkg("GxShapeKit");
  Tcl::defFieldContainer<House>(pkg, SRC_POS);
  Tcl::defCreator<House>(pkg);

  PKG_RETURN;
}

static const char vcid_housetcl_cc[] = "$Id$ $URL$";
#endif // !HOUSETCL_CC_DEFINED
