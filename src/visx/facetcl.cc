///////////////////////////////////////////////////////////////////////
//
// facetcl.cc
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Mon Jan  4 08:00:00 1999
// written: Wed Mar 19 18:00:56 2003
// $Id$
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

class InnerFace : public Face
{
public:
  static InnerFace* make()
  {
    InnerFace* p = new InnerFace;
    p->setField("partsMask", Tcl::toTcl(1));
    return p;
  }
};

class FilledFace : public Face
{
public:
  static FilledFace* make()
  {
    FilledFace* p = new FilledFace;
    p->setField("isFilled", Tcl::toTcl(1));
    return p;
  }
};

extern "C"
int Face_Init(Tcl_Interp* interp)
{
DOTRACE("Face_Init");

  Tcl::Pkg* pkg = new Tcl::Pkg(interp, "Face", "$Revision$");
  Tcl::defFieldContainer<Face>(pkg);
  Tcl::defCreator<Face>(pkg);

  Tcl::Pkg* pkg2 = new Tcl::Pkg(interp, "CloneFace", "$Revision$");
  Tcl::defFieldContainer<CloneFace>(pkg2);
  Tcl::defCreator<CloneFace>(pkg2);

  Tcl::Pkg* pkg3 = new Tcl::Pkg(interp, "InnerFace", "$Revision$");
  Tcl::defFieldContainer<InnerFace>(pkg3);
  Tcl::defCreator<InnerFace>(pkg3);

  Tcl::Pkg* pkg4 = new Tcl::Pkg(interp, "FilledFace", "$Revision$");
  Tcl::defFieldContainer<FilledFace>(pkg4);
  Tcl::defCreator<FilledFace>(pkg4);

  return Tcl::Pkg::initStatus(pkg, pkg2, pkg3, pkg4);
}

static const char vcid_facetcl_cc[] = "$Header$";
#endif // !FACETCL_CC_DEFINED
