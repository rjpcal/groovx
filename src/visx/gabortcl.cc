///////////////////////////////////////////////////////////////////////
//
// gabortcl.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Oct  6 14:16:30 1999
// written: Mon Oct 30 11:12:29 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GABORTCL_CC_DEFINED
#define GABORTCL_CC_DEFINED

#include "gabor.h"

#include "tcl/propitempkg.h"

#define NO_TRACE
#include "util/trace.h"

extern "C"
int Gabor_Init(Tcl_Interp* interp) {
DOTRACE("Gabor_Init");

  Tcl::TclPkg* pkg =
	 new Tcl::PropItemPkg<Gabor>(interp, "Gabor", "$Revision$");

  return pkg->initStatus();
}

static const char vcid_gabortcl_cc[] = "$Header$";
#endif // !GABORTCL_CC_DEFINED
