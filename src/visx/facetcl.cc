///////////////////////////////////////////////////////////////////////
//
// facetcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Jan-99
// written: Thu May 10 12:04:48 2001
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
int Face_Init(Tcl_Interp* interp) {
DOTRACE("Face_Init");

  Tcl::TclPkg* pkg =
	 new Tcl::FieldCntrPkg<Face>(interp, "Face", "$Revision$"); 

  Tcl::TclPkg* pkg2 =
	 new Tcl::FieldCntrPkg<CloneFace>(interp, "CloneFace", "$Revision$"); 

  return pkg->combineStatus(pkg2->initStatus());
}

static const char vcid_facetcl_cc[] = "$Header$";
#endif // !FACETCL_CC_DEFINED
