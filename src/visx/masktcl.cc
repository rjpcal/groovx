///////////////////////////////////////////////////////////////////////
//
// masktcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Sep 23 18:19:05 1999
// written: Thu May 10 12:04:46 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef MASKTCL_CC_DEFINED
#define MASKTCL_CC_DEFINED

#include "maskhatch.h"

#include "tcl/fieldpkg.h"

#define NO_TRACE
#include "util/trace.h"

extern "C"
int Mask_Init(Tcl_Interp* interp) {
DOTRACE("Mask_Init");

  Tcl::TclPkg* pkg =
	 new Tcl::FieldCntrPkg<MaskHatch>(interp, "MaskHatch", "$Revision$");

  return pkg->initStatus();
}

static const char vcid_masktcl_cc[] = "$Header$";
#endif // !MASKTCL_CC_DEFINED
