///////////////////////////////////////////////////////////////////////
//
// masktcl.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Sep 23 18:19:05 1999
// written: Mon Oct 30 11:12:29 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef MASKTCL_CC_DEFINED
#define MASKTCL_CC_DEFINED

#include "maskhatch.h"

#include "tcl/propitempkg.h"

#define NO_TRACE
#include "util/trace.h"


extern "C"
int Mask_Init(Tcl_Interp* interp) {
DOTRACE("Mask_Init");

  Tcl::TclPkg* pkg =
	 new Tcl::PropItemPkg<MaskHatch>(interp, "MaskHatch", "$Revision$");

  return pkg->initStatus();
}

static const char vcid_masktcl_cc[] = "$Header$";
#endif // !MASKTCL_CC_DEFINED
