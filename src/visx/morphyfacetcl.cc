///////////////////////////////////////////////////////////////////////
//
// morphyfacetcl.cc
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Sep  8 15:42:36 1999
// written: Mon Nov 13 22:23:22 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef MORPHYFACETCL_CC_DEFINED
#define MORPHYFACETCL_CC_DEFINED

#include "morphyface.h"

#include "tcl/fieldpkg.h"

#define NO_TRACE
#include "util/trace.h"

extern "C"
int Morphyface_Init(Tcl_Interp* interp) {
DOTRACE("Morphyface_Init");

  Tcl::TclPkg* pkg = 
	 new Tcl::FieldCntrPkg<MorphyFace>(interp, "MorphyFace", "$Revision$");

  return pkg->initStatus();
}

static const char vcid_morphyfacetcl_cc[] = "$Header$";
#endif // !MORPHYFACETCL_CC_DEFINED
