///////////////////////////////////////////////////////////////////////
//
// tclcmd.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Fri Jun 11 14:50:58 1999
// written: Fri Jul 16 17:31:58 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLCMD_CC_DEFINED
#define TCLCMD_CC_DEFINED

#include "tclcmd.h"

#include <typeinfo>
#include <string>

#include "errmsg.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

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

long TclCmd::getLongFromArg(int argn) {
DOTRACE("TclCmd::getLongFromArg");
  long val;
  if ( Tcl_GetLongFromObj(itsInterp, itsObjv[argn], &val) != TCL_OK) {
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

double TclCmd::getDoubleFromArg(int argn) {
DOTRACE("TclCmd::getDoubleFromArg");
  DebugEvalNL(Tcl_GetString(itsObjv[argn]));

  double val;
  try {
	 if ( Tcl_GetDoubleFromObj(itsInterp, itsObjv[argn], &val) != TCL_OK ) {
		throw TclError();
	 }
  }
  catch (TclError&) {
	 throw;
  }
  return val;
}

const char* TclCmd::getCstringFromArg(int argn) {
DOTRACE("TclCmd::getCstringFromArg");
  return Tcl_GetString(itsObjv[argn]);
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

void TclCmd::returnLong(long val) {
DOTRACE("TclCmd::returnLong");
  Tcl_SetObjResult(itsInterp, Tcl_NewLongObj(val));
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

void TclCmd::lappendVal(int val) {
DOTRACE("TclCmd::lappendVal");
  Tcl_Obj* result = Tcl_GetObjResult(itsInterp); 
  int cmd_result = Tcl_ListObjAppendElement(itsInterp, result, 
														  Tcl_NewIntObj(val));
  if (cmd_result != TCL_OK) throw TclError();
}

void TclCmd::lappendVal(long val) {
DOTRACE("TclCmd::lappendVal");
  Tcl_Obj* result = Tcl_GetObjResult(itsInterp);
  int cmd_result = Tcl_ListObjAppendElement(itsInterp, result,
														  Tcl_NewLongObj(val));
  if (cmd_result != TCL_OK) throw TclError();
}

void TclCmd::lappendVal(bool val) {
DOTRACE("TclCmd::lappendVal");
  Tcl_Obj* result = Tcl_GetObjResult(itsInterp); 
  int cmd_result = Tcl_ListObjAppendElement(itsInterp, result, 
														  Tcl_NewBooleanObj(val));
  if (cmd_result != TCL_OK) throw TclError();
}

void TclCmd::lappendVal(double val) {
DOTRACE("TclCmd::lappendVal");
  Tcl_Obj* result = Tcl_GetObjResult(itsInterp); 
  int cmd_result = Tcl_ListObjAppendElement(itsInterp, result,
														  Tcl_NewDoubleObj(val));
  if (cmd_result != TCL_OK) throw TclError();
}

void TclCmd::lappendVal(const char* val) {
DOTRACE("TclCmd::lappendVal");
  Tcl_Obj* result = Tcl_GetObjResult(itsInterp); 
  int cmd_result = Tcl_ListObjAppendElement(itsInterp, result,
														  Tcl_NewStringObj(val, -1));
  if (cmd_result != TCL_OK) throw TclError();
}

void TclCmd::lappendVal(const string& val) {
DOTRACE("TclCmd::lappendVal");
  Tcl_Obj* result = Tcl_GetObjResult(itsInterp); 
  int cmd_result = Tcl_ListObjAppendElement(itsInterp, result,
														  Tcl_NewStringObj(val.c_str(), -1));
  if (cmd_result != TCL_OK) throw TclError();
}

int TclCmd::dummyInvoke(ClientData clientData, Tcl_Interp* interp,
								int objc, Tcl_Obj *const objv[]) {
DOTRACE("TclCmd::dummyInvoke");

  TclCmd* theCmd = static_cast<TclCmd*>(clientData);

  DebugEval((void *) theCmd);
  DebugEvalNL(typeid(*theCmd).name());

  theCmd->itsInterp = interp;
  theCmd->itsObjc = objc;
  theCmd->itsObjv = objv;
  theCmd->itsResult = TCL_OK;

  DebugEval(objc);
  DebugEval(theCmd->itsObjcMin);
  DebugEvalNL(theCmd->itsObjcMax);

  // Check for bad argument count, if so abort the command and return
  // TCL_ERROR...
  if ( ((theCmd->itsExactObjc == true) && 
		  (objc != theCmd->itsObjcMin && objc != theCmd->itsObjcMax))
		 ||
		 ((theCmd->itsExactObjc == false) &&
		  (objc < theCmd->itsObjcMin || objc > theCmd->itsObjcMax)) ) {

	 theCmd->usage();
	 theCmd->itsResult = TCL_ERROR;
	 return TCL_ERROR;
  }
  // ...otherwise if the argument count is OK, try the command and
  // catch all possible exceptions
  try {
	 theCmd->invoke();
  }
  catch (TclError& err) {
	 DebugPrintNL("catch (TclError&)");
	 if (err.msg() != "") {
		err_message(interp, theCmd->itsObjv, err.msg().c_str());
	 }
	 theCmd->itsResult = TCL_ERROR;
  }
  catch (ErrorWithMsg& err) {
	 DebugPrintNL("catch (ErrorWithMsg&)");
	 if (err.msg() != "") {
		err_message(interp, theCmd->itsObjv, err.msg().c_str());
	 }
	 else {
		string msg = "an error of type ";
		msg += typeid(err).name();
		msg += " occurred";
		err_message(interp, theCmd->itsObjv, msg.c_str());
	 }
	 theCmd->itsResult = TCL_ERROR;
  }
  catch (Error& err) {
 	 DebugPrintNL("catch (Error&)");
	 string msg = "an error of type ";
	 msg += typeid(err).name();
	 msg += " occurred";
 	 err_message(interp, theCmd->itsObjv, msg.c_str());
 	 theCmd->itsResult = TCL_ERROR;
  }
  catch (...) {
 	 DebugPrintNL("catch (...)");
 	 err_message(interp, theCmd->itsObjv, "an error of unknown type occurred");
 	 theCmd->itsResult = TCL_ERROR;
  }
  DebugEvalNL(theCmd->itsResult == TCL_OK);
  return theCmd->itsResult;
}


static const char vcid_tclcmd_cc[] = "$Header$";
#endif // !TCLCMD_CC_DEFINED
