///////////////////////////////////////////////////////////////////////
//
// tclcmd.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Fri Jun 11 14:50:58 1999
// written: Sat Mar  4 02:15:41 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLCMD_CC_DEFINED
#define TCLCMD_CC_DEFINED

#include "tclcmd.h"

#include "demangle.h"
#include "errmsg.h"

#include <exception>
#include <typeinfo>
#include <string>
#include <vector>

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

namespace {
  Tcl_Obj* nullObject () {
	 static Tcl_Obj* obj = 0;
	 if (obj == 0) {
		obj = Tcl_NewObj();
		Tcl_IncrRefCount(obj);
	 }
	 return obj;
  }

  void dummyDeleteProc(ClientData clientData) {
	 Tcl::TclCmd* cmd = static_cast<Tcl::TclCmd*>(clientData);
	 delete cmd;
  }
}

class Tcl::TclCmd::Impl {
public:
  vector<TclValue> itsArgs;
};

Tcl::TclCmd::~TclCmd() {
DOTRACE("Tcl::TclCmd::~TclCmd");
  delete itsImpl;
}

Tcl::TclCmd::TclCmd(Tcl_Interp* interp, const char* cmd_name, const char* usage, 
					int objc_min, int objc_max, bool exact_objc) :
  itsUsage(usage),
  itsObjcMin(objc_min),
  itsObjcMax( (objc_max > 0) ? objc_max : objc_min),
  itsExactObjc(exact_objc),
  itsInterp(0),
  itsObjc(0),
  itsObjv(0),
  itsImpl(new Impl),
  itsResult(TCL_OK)
{
DOTRACE("Tcl::TclCmd::TclCmd");
  Tcl_CreateObjCommand(interp,
							  const_cast<char *>(cmd_name),
							  dummyInvoke,
							  static_cast<ClientData> (this),
							  (Tcl_CmdDeleteProc*) NULL);
}

void Tcl::TclCmd::usage() {
DOTRACE("Tcl::TclCmd::usage");
  Tcl_WrongNumArgs(itsInterp, 1, itsObjv, 
						 const_cast<char*>(itsUsage));
  itsResult = TCL_ERROR;
}

void Tcl::TclCmd::errorMessage(const char* msg) {
DOTRACE("Tcl::TclCmd::errorMessage");
  Tcl::err_message(itsInterp, itsObjv, msg);
}


Tcl::TclValue& Tcl::TclCmd::arg(int argn) {
DOTRACE("Tcl::TclCmd::arg");
  if (argn > itsObjc) { throw TclError("argument number too high"); }
  return itsImpl->itsArgs[argn];
}

int Tcl::TclCmd::getIntFromArg(int argn) {
DOTRACE("Tcl::TclCmd::getIntFromArg");
  int val;
  if ( Tcl_GetIntFromObj(itsInterp, itsObjv[argn], &val) != TCL_OK ) {
	 throw TclError();
  }
  return val;
}

long Tcl::TclCmd::getLongFromArg(int argn) {
DOTRACE("Tcl::TclCmd::getLongFromArg");
  long val;
  if ( Tcl_GetLongFromObj(itsInterp, itsObjv[argn], &val) != TCL_OK) {
	 throw TclError();
  }
  return val;
}

bool Tcl::TclCmd::getBoolFromArg(int argn) {
DOTRACE("Tcl::TclCmd::getBoolFromArg");
  int val;
  if ( Tcl_GetBooleanFromObj(itsInterp, itsObjv[argn], &val) != TCL_OK ) {
	 throw TclError();
  }
  return bool(val);
}

double Tcl::TclCmd::getDoubleFromArg(int argn) {
DOTRACE("Tcl::TclCmd::getDoubleFromArg");
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

const char* Tcl::TclCmd::getCstringFromArg(int argn) {
DOTRACE("Tcl::TclCmd::getCstringFromArg");
  return Tcl_GetString(itsObjv[argn]);
}

string Tcl::TclCmd::getStringFromArg(int argn) {
DOTRACE("Tcl::TclCmd::getStringFromArg");
  return Tcl_GetString(itsObjv[argn]);
}

template <>
void Tcl::TclCmd::getValFromObj<int>(Tcl_Obj* obj, int& val) {
  if ( Tcl_GetIntFromObj(itsInterp, obj, &val) != TCL_OK ) throw TclError();
}

template <>
void Tcl::TclCmd::getValFromObj<bool>(Tcl_Obj* obj, bool& val) {
  int int_val;
  if ( Tcl_GetBooleanFromObj(itsInterp, obj, &int_val) != TCL_OK )
	 throw TclError();
  val = bool(int_val);
}

template <>
void Tcl::TclCmd::getValFromObj<double>(Tcl_Obj* obj,
															  double& val) {
  if ( Tcl_GetDoubleFromObj(itsInterp, obj, &val) != TCL_OK )
	 throw TclError();
}

template <>
void Tcl::TclCmd::getValFromObj<const char*>(Tcl_Obj* obj,
																	 const char*& val) {
  val = Tcl_GetString(obj);
}

template <>
void Tcl::TclCmd::getValFromObj<string>(Tcl_Obj* obj,
													 string& val) {
  val = Tcl_GetString(obj);
}

void Tcl::TclCmd::returnVal(const Value& val) {
DOTRACE("Tcl::TclCmd::returnVal");
  TclValue return_val(itsInterp, val);
  Tcl_SetObjResult(itsInterp, return_val.getObj());
  itsResult = TCL_OK;
};

void Tcl::TclCmd::returnVoid() {
DOTRACE("Tcl::TclCmd::returnVoid");
  Tcl_ResetResult(itsInterp); 
  itsResult = TCL_OK;
}

void Tcl::TclCmd::returnError() {
DOTRACE("Tcl::TclCmd::returnError");
  itsResult = TCL_ERROR;
}

void Tcl::TclCmd::returnInt(int val) {
DOTRACE("Tcl::TclCmd::returnInt");
  Tcl_SetObjResult(itsInterp, Tcl_NewIntObj(val));
  itsResult = TCL_OK;
}

void Tcl::TclCmd::returnLong(long val) {
DOTRACE("Tcl::TclCmd::returnLong");
  Tcl_SetObjResult(itsInterp, Tcl_NewLongObj(val));
  itsResult = TCL_OK;
}

void Tcl::TclCmd::returnBool(bool val) {
DOTRACE("Tcl::TclCmd::returnBool");
  Tcl_SetObjResult(itsInterp, Tcl_NewBooleanObj(val));
  itsResult = TCL_OK;
}

void Tcl::TclCmd::returnDouble(double val) {
DOTRACE("Tcl::TclCmd::returnDouble");
  Tcl_SetObjResult(itsInterp, Tcl_NewDoubleObj(val));
  itsResult = TCL_OK;
}

void Tcl::TclCmd::returnCstring(const char* val) {
DOTRACE("Tcl::TclCmd::returnCstring");
  Tcl_SetObjResult(itsInterp, Tcl_NewStringObj(val, -1));
  itsResult = TCL_OK;
}

void Tcl::TclCmd::returnString(const string& val) {
DOTRACE("Tcl::TclCmd::returnString");
  Tcl_SetObjResult(itsInterp, Tcl_NewStringObj(val.c_str(), -1));
  itsResult = TCL_OK;
}

void Tcl::TclCmd::lappendVal(const Value& val) {
DOTRACE("Tcl::TclCmd::lappendVal");
  Tcl_Obj* result = Tcl_GetObjResult(itsInterp);
  TclValue list_element(itsInterp, val);
  int cmd_result = Tcl_ListObjAppendElement(itsInterp, result,
														  list_element.getObj());
  if (cmd_result != TCL_OK) throw TclError();
}

void Tcl::TclCmd::lappendVal(int val) {
DOTRACE("Tcl::TclCmd::lappendVal");
  Tcl_Obj* result = Tcl_GetObjResult(itsInterp); 
  int cmd_result = Tcl_ListObjAppendElement(itsInterp, result, 
														  Tcl_NewIntObj(val));
  if (cmd_result != TCL_OK) throw TclError();
}

void Tcl::TclCmd::lappendVal(long val) {
DOTRACE("Tcl::TclCmd::lappendVal");
  Tcl_Obj* result = Tcl_GetObjResult(itsInterp);
  int cmd_result = Tcl_ListObjAppendElement(itsInterp, result,
														  Tcl_NewLongObj(val));
  if (cmd_result != TCL_OK) throw TclError();
}

void Tcl::TclCmd::lappendVal(bool val) {
DOTRACE("Tcl::TclCmd::lappendVal");
  Tcl_Obj* result = Tcl_GetObjResult(itsInterp); 
  int cmd_result = Tcl_ListObjAppendElement(itsInterp, result, 
														  Tcl_NewBooleanObj(val));
  if (cmd_result != TCL_OK) throw TclError();
}

void Tcl::TclCmd::lappendVal(double val) {
DOTRACE("Tcl::TclCmd::lappendVal");
  Tcl_Obj* result = Tcl_GetObjResult(itsInterp); 
  int cmd_result = Tcl_ListObjAppendElement(itsInterp, result,
														  Tcl_NewDoubleObj(val));
  if (cmd_result != TCL_OK) throw TclError();
}

void Tcl::TclCmd::lappendVal(const char* val) {
DOTRACE("Tcl::TclCmd::lappendVal");
  Tcl_Obj* result = Tcl_GetObjResult(itsInterp); 
  int cmd_result = Tcl_ListObjAppendElement(itsInterp, result,
														  Tcl_NewStringObj(val, -1));
  if (cmd_result != TCL_OK) throw TclError();
}

void Tcl::TclCmd::lappendVal(const string& val) {
DOTRACE("Tcl::TclCmd::lappendVal");
  Tcl_Obj* result = Tcl_GetObjResult(itsInterp); 
  int cmd_result = Tcl_ListObjAppendElement(itsInterp, result,
														  Tcl_NewStringObj(val.c_str(), -1));
  if (cmd_result != TCL_OK) throw TclError();
}

int Tcl::TclCmd::dummyInvoke(ClientData clientData, Tcl_Interp* interp,
									  int objc, Tcl_Obj *const objv[]) {
DOTRACE("Tcl::TclCmd::dummyInvoke");

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
	 
	 if (theCmd->itsImpl->itsArgs.size() == 0) {
		Tcl_Obj* init = Tcl_NewObj();
		theCmd->itsImpl->itsArgs.resize(theCmd->itsObjcMax,
												  TclValue(interp, init));
	 }
	 for (int i=0; i < objc; ++i) {
		theCmd->itsImpl->itsArgs[i].setObj(objv[i]);
	 }

	 theCmd->invoke();
  }
  catch (TclError& err) {
	 DebugPrintNL("catch (TclError&)");
	 if (err.msg() != "") {
		Tcl::err_message(interp, theCmd->itsObjv, err.msg().c_str());
	 }
	 theCmd->itsResult = TCL_ERROR;
  }
  catch (ErrorWithMsg& err) {
	 DebugPrintNL("catch (ErrorWithMsg&)");
	 if (err.msg() != "") {
		Tcl::err_message(interp, theCmd->itsObjv, err.msg().c_str());
	 }
	 else {
		string msg = "an error of type ";
		msg += demangle(typeid(err).name());
		msg += " occurred";
		Tcl::err_message(interp, theCmd->itsObjv, msg.c_str());
	 }
	 theCmd->itsResult = TCL_ERROR;
  }
  catch (Error& err) {
 	 DebugPrintNL("catch (Error&)");
	 string msg = "an error of type ";
	 msg += demangle(typeid(err).name());
	 msg += " occurred";
 	 Tcl::err_message(interp, theCmd->itsObjv, msg.c_str());
 	 theCmd->itsResult = TCL_ERROR;
  }
  catch (exception& err) {
	 string msg = "an error of type ";
	 msg += demangle(typeid(err).name());
	 msg += " occurred: ";
	 msg += err.what();
	 Tcl::err_message(interp, theCmd->itsObjv, msg.c_str());
	 theCmd->itsResult = TCL_ERROR;
  }
  catch (...) {
 	 DebugPrintNL("catch (...)");
 	 Tcl::err_message(interp, theCmd->itsObjv, "an error of unknown type occurred");
 	 theCmd->itsResult = TCL_ERROR;
  }


  // cleanup
  for (int i=0; i < objc; ++i) {
 	 theCmd->itsImpl->itsArgs[i].setObj(nullObject());
  }
  theCmd->itsObjc = 0;

  DebugEvalNL(theCmd->itsResult == TCL_OK);
  return theCmd->itsResult;
}

static const char vcid_tclcmd_cc[] = "$Header$";
#endif // !TCLCMD_CC_DEFINED
