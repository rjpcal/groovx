///////////////////////////////////////////////////////////////////////
//
// expttesttcl.cc
// Rob Peters
// created: Tue May 11 13:13:41 1999
// written: Wed Mar  8 16:29:15 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef EXPTTESTTCL_CC_DEFINED
#define EXPTTESTTCL_CC_DEFINED

#include "tcllink.h"

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
