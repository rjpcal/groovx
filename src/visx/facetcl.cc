///////////////////////////////////////////////////////////////////////
//
// facetcl.cc
//
// Copyright (c) 1999-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Mon Jan  4 08:00:00 1999
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

#ifndef FACETCL_CC_DEFINED
#define FACETCL_CC_DEFINED

#include "io/fieldpkg.h"

#include "visx/cloneface.h"
#include "visx/face.h"

#include "util/trace.h"

namespace
{
  static Face* makeInnerFace()
  {
    Face* p = Face::make();
    p->setField("partsMask", Tcl::toTcl(1));
    return p;
  }

  static Face* makeFilledFace()
  {
    Face* p = Face::make();
    p->setField("isFilled", Tcl::toTcl(1));
    return p;
  }
}

extern "C"
int Face_Init(Tcl_Interp* interp)
{
DOTRACE("Face_Init");

  PKG_CREATE(interp, "Face", "4.$Revision$");

  pkg->inheritPkg("GxShapeKit");
  Tcl::defFieldContainer<Face>(pkg, SRC_POS);
  Tcl::defCreator<Face>(pkg);

  Nub::ObjFactory::theOne().register_creator( &makeInnerFace,
                                              "InnerFace" );

  Nub::ObjFactory::theOne().register_creator( &makeFilledFace,
                                              "FilledFace" );

  PKG_RETURN;
}

extern "C"
int Cloneface_Init(Tcl_Interp* interp)
{
DOTRACE("Cloneface_Init");

  PKG_CREATE(interp, "CloneFace", "4.$Revision$");
  pkg->inheritPkg("Face");
  Tcl::defFieldContainer<CloneFace>(pkg, SRC_POS);
  Tcl::defCreator<CloneFace>(pkg);

  PKG_RETURN;
}

static const char vcid_facetcl_cc[] = "$Id$ $URL$";
#endif // !FACETCL_CC_DEFINED
