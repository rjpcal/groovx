///////////////////////////////////////////////////////////////////////
//
// facetcl.cc
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Jan-99
// written: Mon Nov 13 21:10:09 2000
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

  IO::IoFactory::theOne().registerCreatorFunc(&CloneFace::make);

  return pkg->initStatus();
}

static const char vcid_facetcl_cc[] = "$Header$";
#endif // !FACETCL_CC_DEFINED
