///////////////////////////////////////////////////////////////////////
//
// fishtcl.cc
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Wed Sep 29 12:00:53 1999
// commit: $Id$
// $HeadURL$
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

#ifndef GROOVX_VISX_FISHTCL_CC_UTC20050626084017_DEFINED
#define GROOVX_VISX_FISHTCL_CC_UTC20050626084017_DEFINED

#include "visx/fishtcl.h"

#include "io/fieldpkg.h"

#include "nub/objfactory.h"

#include "tcl/objpkg.h"
#include "tcl/tracertcl.h"

#include "visx/fish.h"

#include "rutz/trace.h"

namespace
{
  Nub::UID makeFish(const char* spline_file, const char* coord_file,
                    int index)
  {
    Nub::Ref<Fish> obj(Fish::makeFromFiles(spline_file, coord_file, index));
    return obj.id();
  }
}

extern "C"
int Fish_Init(Tcl_Interp* interp)
{
DOTRACE("Fish_Init");

  PKG_CREATE(interp, "Fish", "4.$Revision$");
  pkg->inheritPkg("GxShapeKit");

  Tcl::defTracing(pkg, Fish::tracer);

  Tcl::defFieldContainer<Fish>(pkg, SRC_POS);

  pkg->defVec( "make", "spline_file coord_file index", &makeFish, 1,
               SRC_POS );

  Nub::ObjFactory::theOne().register_creator(&Fish::make);

  PKG_RETURN;
}

static const char vcid_groovx_visx_fishtcl_cc_utc20050626084017[] = "$Id$ $HeadURL$";
#endif // !GROOVX_VISX_FISHTCL_CC_UTC20050626084017_DEFINED
