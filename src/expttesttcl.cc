///////////////////////////////////////////////////////////////////////
//
// expttesttcl.cc
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue May 11 13:13:41 1999
// written: Fri Nov 10 17:27:06 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef EXPTTESTTCL_CC_DEFINED
#define EXPTTESTTCL_CC_DEFINED

#include "tcl/tcllink.h"

//--------------------------------------------------------------------
//
// ExptTestTcl::Expttest_Init --
//
//--------------------------------------------------------------------

extern "C"
int Expttest_Init(Tcl_Interp* interp) {
  static int haveTest=0;

  Tcl::Tcl_LinkBoolean(interp, "Expt::haveTest", &haveTest, TCL_LINK_READ_ONLY);
  
  return TCL_OK;
}

static const char vcid_expttesttcl_cc[] = "$Header$";
#endif // !EXPTTESTTCL_CC_DEFINED
