///////////////////////////////////////////////////////////////////////
//
// facetcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Jan-99
// written: Wed Jul 18 11:03:13 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FACETCL_CC_DEFINED
#define FACETCL_CC_DEFINED

#include "cloneface.h"
#include "face.h"

#include "tcl/fieldpkg.h"

#define NO_TRACE
#include "util/trace.h"

extern "C"
int Face_Init(Tcl_Interp* interp)
{
DOTRACE("Face_Init");

  Tcl::Pkg* pkg = new Tcl::Pkg(interp, "Face", "$Revision$");
  Tcl::defFieldContainer<Face>(pkg);

  Tcl::Pkg* pkg2 = new Tcl::Pkg(interp, "CloneFace", "$Revision$");
  Tcl::defFieldContainer<CloneFace>(pkg);

  return Tcl::Pkg::initStatus(pkg, pkg2);
}

static const char vcid_facetcl_cc[] = "$Header$";
#endif // !FACETCL_CC_DEFINED
