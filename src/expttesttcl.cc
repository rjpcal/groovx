///////////////////////////////////////////////////////////////////////
//
// expttesttcl.cc
// Rob Peters
// created: Tue May 11 13:13:41 1999
// written: Tue Jul 20 15:04:26 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef EXPTTESTTCL_CC_DEFINED
#define EXPTTESTTCL_CC_DEFINED

#include <tcl.h>
#include <vector>

#include "tcllink.h"

//--------------------------------------------------------------------
//
// ExptTestTcl::Expttest_Init --
//
//--------------------------------------------------------------------

extern "C" int Expttest_Init(Tcl_Interp* interp);

int Expttest_Init(Tcl_Interp* interp) {
  static int haveTest=0;

  Tcl_LinkBoolean(interp, "Expt::haveTest", &haveTest, TCL_LINK_READ_ONLY);
  
  return TCL_OK;
}

static const char vcid_expttesttcl_cc[] = "$Header$";
#endif // !EXPTTESTTCL_CC_DEFINED
