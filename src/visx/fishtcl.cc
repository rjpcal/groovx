///////////////////////////////////////////////////////////////////////
//
// fishtcl.cc
//
// Copyright (c) 1999-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Wed Sep 29 12:00:53 1999
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

#ifndef FISHTCL_CC_DEFINED
#define FISHTCL_CC_DEFINED

#include "io/fieldpkg.h"

#include "tcl/objpkg.h"
#include "tcl/tracertcl.h"

#include "util/objfactory.h"

#include "visx/fish.h"

#include "util/trace.h"

namespace
{
  Util::UID makeFish(const char* spline_file, const char* coord_file,
                     int index)
  {
    Util::Ref<Fish> obj(Fish::makeFromFiles(spline_file, coord_file, index));
    return obj.id();
  }
}

extern "C"
int Fish_Init(Tcl_Interp* interp)
{
DOTRACE("Fish_Init");

  PKG_CREATE(interp, "Fish", "$Revision$");
  pkg->inheritPkg("GxShapeKit");

  Tcl::defTracing(pkg, Fish::tracer);

  Tcl::defFieldContainer<Fish>(pkg, SRC_POS);

  pkg->defVec( "make", "spline_file coord_file index", &makeFish, 1,
               SRC_POS );

  Util::ObjFactory::theOne().register_creator(&Fish::make);

  PKG_RETURN;
}

static const char vcid_fishtcl_cc[] = "$Header$";
#endif // !FISHTCL_CC_DEFINED
