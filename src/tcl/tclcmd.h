///////////////////////////////////////////////////////////////////////
//
// tclcmd.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Fri Jun 11 14:50:43 1999
// written: Fri Jul 16 17:18:43 1999
// $Id$
//
// This file defines the abstract class TclCmd, which provides
// a way to wrap Tcl object commands in C++ classes. It allows
// commands that do similar things to be related through inheritance,
// and for common behavior to be placed in base classes. The
// TclCmd class itself takes care of such things as checking
// the argument count, and provides utility functions such as
// getIntFromArg() and getCstringFromArg() for getting basic types from
// a command argument, as well as functions such as returnInt() and
// returnCstring() for returning basic types to Tcl throught the
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

#ifndef TCLERROR_H_DEFINED
#include "tclerror.h"
#endif

class TclCmd {
public:
  // If exact_objc is true, then the objc of a command invocation is
  // required to exactly equal either objc_min or objc_max; if it is
  // false, then objc must be between objc_min and objc_max, inclusive.
  TclCmd(Tcl_Interp* interp, const char* cmd_name, const char* usage, 
			int objc_min=0, int objc_max=100000, bool exact_objc=false);
  virtual ~TclCmd();

  static int dummyInvoke(ClientData clientData, Tcl_Interp* interp,
								 int objc, Tcl_Obj *const objv[]);

protected:
  virtual void invoke() = 0;

  virtual void usage();

  void errorMessage(const char* msg);

  int objc() { return itsObjc; }

  int    getIntFromArg(int argn);
  long   getLongFromArg(int argn);
  bool   getBoolFromArg(int argn);
  double getDoubleFromArg(int argn);
  const char*  getCstringFromArg(int argn);
  string getStringFromArg(int argn) { return string(getCstringFromArg(argn)); }

  template <class T>
  void getValFromArg(int argn, T& val)
	 { getValFromObj(itsObjv[argn], val); }

  template <class T, class Iterator>
  void getSequenceFromArg(int argn, Iterator itr, T* dummy) {
	 Tcl_Obj** elements;
	 int count;
	 if ( Tcl_ListObjGetElements(itsInterp, itsObjv[argn], &count, &elements)
			!= TCL_OK) {
		throw TclError();
	 }
	 for (int i = 0; i < count; ++i) {
		T val;
		getValFromObj(elements[i], val);
		*itr = val;
	 }
  }

  void returnVoid();
  void returnError();
  void returnInt(int val);
  void returnLong(long val);
  void returnBool(bool val);
  void returnDouble(double val);
  void returnCstring(const char* val);
  void returnString(const string& val);

  void returnVal(int val) { returnInt(val); }
  void returnVal(long val) { returnInt(val); }
  void returnVal(bool val) { returnBool(val); }
  void returnVal(double val) { returnDouble(val); }
  void returnVal(const char* val) { returnCstring(val); }
  void returnVal(const string& val) { returnString(val); }

  template <class Itr>
  void returnSequence(Itr begin, Itr end) {
	 while (begin != end) {
		lappendVal(*begin);
		++begin;
	 }
  }

  void lappendVal(int val);
  void lappendVal(long val);
  void lappendVal(bool val);
  void lappendVal(double val);
  void lappendVal(const char* val);
  void lappendVal(const string& val);

private:
  template <class T>
  void getValFromObj(Tcl_Obj* obj, T& val);

  template <>
  void getValFromObj<int>(Tcl_Obj* obj, int& val) {
	 if ( Tcl_GetIntFromObj(itsInterp, obj, &val) != TCL_OK ) throw TclError();
  }

  template <>
  void getValFromObj<bool>(Tcl_Obj* obj, bool& val) {
	 int int_val;
	 if ( Tcl_GetBooleanFromObj(itsInterp, obj, &int_val) != TCL_OK )
		throw TclError();
	 val = bool(int_val);
  }

  template <>
  void getValFromObj<double>(Tcl_Obj* obj, double& val) {
	 if ( Tcl_GetDoubleFromObj(itsInterp, obj, &val) != TCL_OK )
		throw TclError();
  }

  template <>
  void getValFromObj<const char*>(Tcl_Obj* obj, const char*& val) {
	 val = Tcl_GetString(obj);
  }

  template <>
  void getValFromObj<string>(Tcl_Obj* obj, string& val) {
	 val = Tcl_GetString(obj);
  }

  // These are set once per command object
  const char* const itsUsage;
  const int itsObjcMin;
  const int itsObjcMax;
  const bool itsExactObjc;

  // These are set once per invocation of the command object
  Tcl_Interp* itsInterp;
  int itsObjc;
  Tcl_Obj* const* itsObjv;

  int itsResult;
};

static const char vcid_tclcmd_h[] = "$Header$";
#endif // !TCLCMD_H_DEFINED
