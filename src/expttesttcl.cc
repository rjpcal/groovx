///////////////////////////////////////////////////////////////////////
//
// expttesttcl.cc
// Rob Peters
// created: Tue May 11 13:13:41 1999
// written: Sat Jun 26 12:55:46 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef EXPTTESTTCL_CC_DEFINED
#define EXPTTESTTCL_CC_DEFINED

#include <tcl.h>

#include "block.h"
#include "tcllink.h"
#include "exptdriver.h"

#define NO_TRACE
#include "trace.h"
#include "debug.h"

namespace {
  Block& getBlock() { return ExptDriver::theExptDriver().block(); }
}

///////////////////////////////////////////////////////////////////////
//
// Expt Tcl Test package declarations & definitions
//
///////////////////////////////////////////////////////////////////////

namespace ExptTestTcl {

  // Tcl command procedures used only for testing the Expt class
#ifdef LOCAL_TEST
  Tcl_ObjCmdProc abortTrialCmd;
  Tcl_ObjCmdProc beginTrialCmd;
  Tcl_ObjCmdProc recordResponseCmd;
  Tcl_ObjCmdProc undrawTrialCmd;
#endif

  // This variable is linked into Tcl, so we can tell from Tcl if
  // testing functions have been enabled.
#ifdef LOCAL_TEST
  int haveTest=1;
#else 
  int haveTest=0;
#endif

}

//--------------------------------------------------------------------
//
// ExptTestTcl::abortTrialCmd --
//
// Abort the current trial of the current experiment. Simply a driver
// for Expt::abortTrial -- see comments for that function.
//
//--------------------------------------------------------------------

#ifdef LOCAL_TEST
int ExptTestTcl::abortTrialCmd(ClientData, Tcl_Interp* interp,
										 int objc, Tcl_Obj* const objv[]) {
DOTRACE("ExptTestTcl::abortTrialCmd");
  if (objc != 1) {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }

  getBlock().abortTrial();
  return TCL_OK;
}
#endif

//--------------------------------------------------------------------
//
// ExptTestTcl::beginTrialCmd --
//
// Start the next trial in the current experiment. Mainly a driver for
// Expt::drawTrial. 
//
// Results: 
// If Expt::verbose is true, a description of the current trial is
// returned, otherwise nothing.
//
// Side effects:
// See comments for Expt::drawTrial.
//
//--------------------------------------------------------------------

#ifdef LOCAL_TEST
int ExptTestTcl::beginTrialCmd(ClientData, Tcl_Interp* interp,
										 int objc, Tcl_Obj* const objv[]) {
DOTRACE("ExptTestTcl::beginTrialCmd");
  if (objc != 1) {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }

  Block& block = getBlock();
  block.drawTrial();

  return TCL_OK;
}
#endif

//--------------------------------------------------------------------
//
// ExptTestTcl::recordResponseCmd --
//
// Record a response to the current trial in the current Expt, and
// prepare the Expt for the next trial. Simply a driver for
// Expt::processResponse -- see comments for that function.
//
//--------------------------------------------------------------------

#ifdef LOCAL_TEST
int ExptTestTcl::recordResponseCmd(ClientData, Tcl_Interp* interp,
											  int objc, Tcl_Obj* const objv[]) {
DOTRACE("ExptTestTcl::recordResponseCmd");
  if (objc != 2) {
    Tcl_WrongNumArgs(interp, 1, objv, "response");
    return TCL_ERROR;
  }

  int response;
  if (Tcl_GetIntFromObj(interp, objv[1], &response) != TCL_OK)
    return TCL_ERROR;

  getBlock().processResponse(response);
  return TCL_OK;
}
#endif

//--------------------------------------------------------------------
//
// ExptTestTcl::undrawTrialCmd --
//
// Erase the current trial from the screen. Simply a driver for
// Expt::undrawTrial -- see comments for tat function.
//
//--------------------------------------------------------------------

#ifdef LOCAL_TEST
int ExptTestTcl::undrawTrialCmd(ClientData, Tcl_Interp* interp,
										  int objc, Tcl_Obj* const objv[]) {
DOTRACE("ExptTestTcl::undrawTrialCmd");
  if (objc > 1) {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }

  getBlock().undrawTrial();
  return TCL_OK;
}
#endif

//--------------------------------------------------------------------
//
// ExptTestTcl::Expttest_Init --
//
//--------------------------------------------------------------------


extern "C" int Expttest_Init(Tcl_Interp* interp);

int Expttest_Init(Tcl_Interp* interp) {
DOTRACE("Expttest_Init");

  using namespace ExptTestTcl;

#ifdef LOCAL_TEST
  struct CmdName_CmdProc {
	 const char* cmdName;
	 Tcl_ObjCmdProc* cmdProc;
  };

  static CmdName_CmdProc Names_Procs[] = {
    { "Expt::abortTrial", abortTrialCmd },
    { "Expt::beginTrial", beginTrialCmd },
    { "Expt::recordResponse", recordResponseCmd },
    { "Expt::undrawTrial", undrawTrialCmd }
  };

  // Add all commands to the ::Expt namespace.
  for (int i = 0; i < sizeof(Names_Procs)/sizeof(CmdName_CmdProc); ++i) {
    Tcl_CreateObjCommand(interp, 
                         const_cast<char *>(Names_Procs[i].cmdName),
                         Names_Procs[i].cmdProc,
                         (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  }
#endif

  Tcl_LinkBoolean(interp, "Expt::haveTest", &haveTest, TCL_LINK_READ_ONLY);
  
  return TCL_OK;
}

static const char vcid_expttesttcl_cc[] = "$Header$";
#endif // !EXPTTESTTCL_CC_DEFINED
