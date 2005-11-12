/** @file visx/tclpkg-fish.cc tcl interface package for class Fish */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Wed Sep 29 12:00:53 1999
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

#ifndef GROOVX_VISX_TCLPKG_FISH_CC_UTC20050628171008_DEFINED
#define GROOVX_VISX_TCLPKG_FISH_CC_UTC20050628171008_DEFINED

#include "visx/tclpkg-fish.h"

#include "nub/objfactory.h"

#include "tcl/objpkg.h"
#include "tcl/tracertcl.h"

#include "tcl-io/fieldpkg.h"

#include "visx/fish.h"

#include "rutz/trace.h"

namespace
{
  nub::uid makeFish(const char* spline_file, const char* coord_file,
                    int index)
  {
    nub::ref<Fish> obj(Fish::makeFromFiles(spline_file, coord_file, index));
    return obj.id();
  }
}

extern "C"
int Fish_Init(Tcl_Interp* interp)
{
GVX_TRACE("Fish_Init");

  GVX_PKG_CREATE(pkg, interp, "Fish", "4.$Revision$");
  pkg->inherit_pkg("GxShapeKit");

  tcl::def_tracing(pkg, Fish::tracer);

  tcl::defFieldContainer<Fish>(pkg, SRC_POS);

  pkg->def_vec( "make", "spline_file coord_file index", &makeFish, 1,
                SRC_POS );

  nub::obj_factory::instance().register_creator(&Fish::make);

  GVX_PKG_RETURN(pkg);
}

static const char vcid_groovx_visx_tclpkg_fish_cc_utc20050628171008[] = "$Id$ $HeadURL$";
#endif // !GROOVX_VISX_TCLPKG_FISH_CC_UTC20050628171008_DEFINED
