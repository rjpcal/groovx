///////////////////////////////////////////////////////////////////////
//
// tclcmd.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Fri Jun 11 14:50:43 1999
// written: Mon Jun 14 12:19:34 1999
// $Id$
//
// This file defines the abstract class TclCmd, which provides
// a way to wrap Tcl object commands in C++ classes. It allows
// commands that do similar things to be related through inheritance,
// and for common behavior to be placed in base classes. The
// TclCmd class itself takes care of such things as checking
// the argument count, and provides utility functions such as
// getIntFromArg() and getStringFromArg() for getting basic types from
// a command argument, as well as functions such as returnInt() and
// returnString() for returning basic types to Tcl throught the
// Tcl_ObjResult.
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLCMD_H_DEFINED
#define TCLCMD_H_DEFINED

#ifndef TCL_H_DEFINED
#include <tcl.h>
#define TCL_H_DEFINED
#endif

#ifndef STRING_DEFINED
#include <string>
#define STRING_DEFINED
#endif

class TclError {
public:
  TclError(const string& msg="") : itsMsg(msg) {}
  virtual ~TclError();
  const string& msg() { return itsMsg; }
private:
  const string itsMsg;
};

class TclCmd {
public:
  // If exact_objc is true, then the objc of a command invocation is
  // required to exactly equal either objc_min or objc_max; if it is
  // false, then objc must be between objc_min and objc_max, inclusive.
  TclCmd(Tcl_Interp* interp, const char* cmd_name, 
					 const char* usage, int objc_min=0, int objc_max=100000,
					 bool exact_objc=false);
  virtual ~TclCmd();

  static int dummyInvoke(ClientData clientData, Tcl_Interp* interp,
								 int objc, Tcl_Obj *const objv[]);

protected:
  virtual void invoke() = 0;

  virtual void usage();

  void errorMessage(const char* msg);

  int objc() { return itsObjc; }

  int    getIntFromArg(int argn);
  bool   getBoolFromArg(int argn);
  double getDoubleFromArg(int argn);
  char*  getStringFromArg(int argn);

  void returnVoid();
  void returnError();
  void returnInt(int val);
  void returnBool(bool val);
  void returnDouble(double val);
  void returnString(const char* val);

private:
  const char* const itsUsage;
  const int itsObjcMin;
  const int itsObjcMax;
  const bool itsExactObjc;

  Tcl_Interp* itsInterp;
  int itsObjc;
  Tcl_Obj* const* itsObjv;

  int itsResult;
};

static const char vcid_tclcmd_h[] = "$Header$";
#endif // !TCLCMD_H_DEFINED
