///////////////////////////////////////////////////////////////////////
//
// gabortcl.cc
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Wed Oct  6 14:16:30 1999
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

#ifndef GABORTCL_CC_DEFINED
#define GABORTCL_CC_DEFINED

#include "visx/gabortcl.h"

#include "io/fieldpkg.h"

#include "visx/gabor.h"
#include "visx/gaborarray.h"

#include "rutz/trace.h"

extern "C"
int Gabor_Init(Tcl_Interp* interp)
{
DOTRACE("Gabor_Init");

  PKG_CREATE(interp, "Gabor", "4.$Revision$");
  pkg->inheritPkg("GxShapeKit");
  Tcl::defFieldContainer<Gabor>(pkg, SRC_POS);
  Tcl::defCreator<Gabor>(pkg);
  pkg->def("saveImage", "objref filename", &Gabor::saveImage, SRC_POS);

  PKG_RETURN;
}

extern "C"
int Gaborarray_Init(Tcl_Interp* interp)
{
DOTRACE("Gaborarray_Init");

  PKG_CREATE(interp, "GaborArray", "4.$Revision$");
  pkg->inheritPkg("GxShapeKit");
  Tcl::defFieldContainer<GaborArray>(pkg, SRC_POS);
  Tcl::defCreator<GaborArray>(pkg);
  pkg->def("saveImage", "objref filename", &GaborArray::saveImage, SRC_POS);
  pkg->def("saveContourOnlyImage", "objref filename",
           &GaborArray::saveContourOnlyImage,
           SRC_POS);

  PKG_RETURN;
}

static const char vcid_gabortcl_cc[] = "$Id$ $URL$";
#endif // !GABORTCL_CC_DEFINED
