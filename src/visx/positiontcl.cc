///////////////////////////////////////////////////////////////////////
//
// positiontcl.cc
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Mar 13 12:53:34 1999
// written: Wed Nov 29 13:29:02 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef POSITIONTCL_CC_DEFINED
#define POSITIONTCL_CC_DEFINED

#include "position.h"

#include "tcl/fieldpkg.h"

#define NO_TRACE
#include "util/trace.h"

extern "C"
int Pos_Init(Tcl_Interp* interp) {
DOTRACE("Pos_Init");

  Tcl::TclPkg* pkg = new Tcl::FieldCntrPkg<Position>(interp, "Pos",
																	  "$Revision$");

  return pkg->initStatus();
}

static const char vcid_positiontcl_cc[] = "$Header$";
#endif // !POSITIONTCL_CC_DEFINED
