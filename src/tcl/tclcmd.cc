///////////////////////////////////////////////////////////////////////
//
// tclcmd.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Fri Jun 11 14:50:58 1999
// written: Wed Jun 16 19:29:17 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLCMD_CC_DEFINED
#define TCLCMD_CC_DEFINED

#include "tclcmd.h"

#include <typeinfo>

#include "errmsg.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

TclError::~TclError() {
DOTRACE("TclError::~TclError");
}

TclCmd::~TclCmd() {
DOTRACE("TclCmd::~TclCmd");
}

TclCmd::TclCmd(Tcl_Interp* interp, const char* cmd_name, const char* usage, 
					int objc_min, int objc_max, bool exact_objc) :
  itsUsage(usage),
  itsObjcMin(objc_min),
  itsObjcMax(objc_max),
  itsExactObjc(exact_objc),
  itsInterp(0),
  itsObjc(0),
  itsObjv(0),
  itsResult(TCL_OK)
{
DOTRACE("TclCmd::TclCmd");
  Tcl_CreateObjCommand(interp,
							  const_cast<char *>(cmd_name),
							  dummyInvoke,
							  static_cast<ClientData> (this),
							  (Tcl_CmdDeleteProc *) NULL);
}

void TclCmd::usage() {
DOTRACE("TclCmd::usage");
  Tcl_WrongNumArgs(itsInterp, 1, itsObjv, 
						 const_cast<char*>(itsUsage));
  itsResult = TCL_ERROR;
}

void TclCmd::errorMessage(const char* msg) {
DOTRACE("TclCmd::errorMessage");
  err_message(itsInterp, itsObjv, msg);
}


int TclCmd::getIntFromArg(int argn) {
DOTRACE("TclCmd::getIntFromArg");
  int val;
  if ( Tcl_GetIntFromObj(itsInterp, itsObjv[argn], &val) != TCL_OK ) {
	 throw TclError();
  }
  return val;
}

bool TclCmd::getBoolFromArg(int argn) {
DOTRACE("TclCmd::getBoolFromArg");
  int val;
  if ( Tcl_GetBooleanFromObj(itsInterp, itsObjv[argn], &val) != TCL_OK ) {
	 throw TclError();
  }
  return bool(val);
}

char* TclCmd::getCstringFromArg(int argn) {
DOTRACE("TclCmd::getCstringFromArg");
  return Tcl_GetString(itsObjv[argn]);
}

double TclCmd::getDoubleFromArg(int argn) {
DOTRACE("TclCmd::getDoubleFromArg");
  double val;
  if ( Tcl_GetDoubleFromObj(itsInterp, itsObjv[argn], &val) != TCL_OK ) {
	 throw TclError();
  }
  return val;
}

void TclCmd::returnVoid() {
DOTRACE("TclCmd::returnVoid");
  itsResult = TCL_OK;
}

void TclCmd::returnError() {
DOTRACE("TclCmd::returnError");
  itsResult = TCL_ERROR;
}

void TclCmd::returnInt(int val) {
DOTRACE("TclCmd::returnInt");
  Tcl_SetObjResult(itsInterp, Tcl_NewIntObj(val));
  itsResult = TCL_OK;
}

void TclCmd::returnBool(bool val) {
DOTRACE("TclCmd::returnBool");
  Tcl_SetObjResult(itsInterp, Tcl_NewBooleanObj(val));
  itsResult = TCL_OK;
}

void TclCmd::returnDouble(double val) {
DOTRACE("TclCmd::returnDouble");
  Tcl_SetObjResult(itsInterp, Tcl_NewDoubleObj(val));
  itsResult = TCL_OK;
}

void TclCmd::returnCstring(const char* val) {
DOTRACE("TclCmd::returnCstring");
  Tcl_SetObjResult(itsInterp, Tcl_NewStringObj(val, -1));
  itsResult = TCL_OK;
}

void TclCmd::returnString(const string& val) {
DOTRACE("TclCmd::returnString");
  Tcl_SetObjResult(itsInterp, Tcl_NewStringObj(val.c_str(), -1));
  itsResult = TCL_OK;
}

int TclCmd::dummyInvoke(ClientData clientData, Tcl_Interp* interp,
										 int objc, Tcl_Obj *const objv[]) {
DOTRACE("TclCmd::dummyInvoke");

  TclCmd* theCmd = static_cast<TclCmd*>(clientData);

  DebugEvalNL(typeid(*theCmd).name());

  theCmd->itsInterp = interp;
  theCmd->itsObjc = objc;
  theCmd->itsObjv = objv;
  theCmd->itsResult = TCL_OK;

  // Check for bad argument count...
  if ( ((theCmd->itsExactObjc == true) && 
		  (objc != theCmd->itsObjcMin && objc != theCmd->itsObjcMax))
		 ||
		 ((theCmd->itsExactObjc == false) &&
		  (objc < theCmd->itsObjcMin || objc > theCmd->itsObjcMax)) ) {

	 theCmd->usage();
  }
  // ...otherwise if the argument count is OK
  else {
  
	 try {
		theCmd->invoke();
	 }
	 catch (TclError& err) {
		if (err.msg() != "") {
		  err_message(interp, theCmd->itsObjv, err.msg().c_str());
		}
		theCmd->itsResult = TCL_ERROR;
	 }
  }
  DebugEvalNL(theCmd->itsResult == TCL_OK);
  return theCmd->itsResult;
}


static const char vcid_tclcmd_cc[] = "$Header$";
#endif // !TCLCMD_CC_DEFINED
