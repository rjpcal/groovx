///////////////////////////////////////////////////////////////////////
//
// facetcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jan  4 08:00:00 1999
// written: Mon Sep 10 17:17:41 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FACETCL_CC_DEFINED
#define FACETCL_CC_DEFINED

#include "visx/cloneface.h"
#include "visx/face.h"

#include "tcl/fieldpkg.h"

#include "util/trace.h"

extern "C"
int Face_Init(Tcl_Interp* interp)
{
DOTRACE("Face_Init");

  Tcl::Pkg* pkg = new Tcl::Pkg(interp, "Face", "$Revision$");
  Tcl::defFieldContainer<Face>(pkg);
  Tcl::defCreator<Face>(pkg);

  Tcl::Pkg* pkg2 = new Tcl::Pkg(interp, "CloneFace", "$Revision$");
  Tcl::defFieldContainer<CloneFace>(pkg2);
  Tcl::defCreator<CloneFace>(pkg);

  return Tcl::Pkg::initStatus(pkg, pkg2);
}

static const char vcid_facetcl_cc[] = "$Header$";
#endif // !FACETCL_CC_DEFINED
