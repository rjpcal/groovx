///////////////////////////////////////////////////////////////////////
//
// facetcl.cc
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Mon Jan  4 08:00:00 1999
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef FACETCL_CC_DEFINED
#define FACETCL_CC_DEFINED

#include "visx/cloneface.h"
#include "visx/face.h"

#include "tcl/fieldpkg.h"

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

  Tcl::Pkg* pkg = new Tcl::Pkg(interp, "Face", "$Revision$");
  pkg->inheritPkg("GxShapeKit");
  Tcl::defFieldContainer<Face>(pkg);
  Tcl::defCreator<Face>(pkg);

  Util::ObjFactory::theOne().registerCreatorFunc( &makeInnerFace,
                                                  "InnerFace" );

  Util::ObjFactory::theOne().registerCreatorFunc( &makeFilledFace,
                                                  "FilledFace" );

  return pkg->initStatus();
}

extern "C"
int Cloneface_Init(Tcl_Interp* interp)
{
DOTRACE("Cloneface_Init");

  Tcl::Pkg* pkg = new Tcl::Pkg(interp, "CloneFace", "$Revision$");
  pkg->inheritPkg("Face");
  Tcl::defFieldContainer<CloneFace>(pkg);
  Tcl::defCreator<CloneFace>(pkg);

  return pkg->initStatus();
}

static const char vcid_facetcl_cc[] = "$Header$";
#endif // !FACETCL_CC_DEFINED
