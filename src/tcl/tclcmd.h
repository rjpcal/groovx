///////////////////////////////////////////////////////////////////////
//
// tclcmd.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Fri Jun 11 14:50:43 1999
// written: Wed Mar  8 14:24:20 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLCMD_H_DEFINED
#define TCLCMD_H_DEFINED

#ifndef TCL_H_DEFINED
#include <tcl.h>
#define TCL_H_DEFINED
#endif

#ifndef TCLVALUE_H_DEFINED
#include "tclvalue.h"
#endif

#ifndef TCLERROR_H_DEFINED
#include "tclerror.h"
#endif

namespace Tcl {
  class TclCmd;
}

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c TclCmd is an abstract class that which provides a way to wrap
 * Tcl object commands in C++ classes. It allows commands that do
 * similar things to be related through inheritance, and for common
 * behavior to be placed in base classes. The \c TclCmd class itself
 * takes care of such things as checking the argument count, and
 * issuing an error message if the argument count is incorrect.
 * 
 * Most of the interface of \c TclCmd is \c protected, as it is
 * intended to be used in the implementation of invoke. This interface
 * provides functions such as \c getIntFromArg() and
 * getCstringFromArg() for getting basic types from a command
 * argument, as well as functions such as \c returnInt() and \c
 * returnCstring() for returning basic types to Tcl.
 *
 * The only C++ clients of \c TclCmd should be those who make
 * subclasses to perform specific actions, and those who instantiate
 * these subclasses to get the commands running in a Tcl
 * interpreter. No general C++ client should ever need to invoke a \c
 * TclCmd member function.
 *
 **/
///////////////////////////////////////////////////////////////////////

class Tcl::TclCmd {
public:
  /** Construct with basic properties for the command. If \a
		exact_objc is true, then the \a objc of a command invocation is
		required to be exactly equal either \a objc_min or \a objc_max;
		if it is false, then \a objc must be between \a objc_min and \a
		objc_max, inclusive. If the value given for \a objc_max is
		negative, then the maximum objc will be set to the same value as
		\a objc_min. */
  TclCmd(Tcl_Interp* interp, const char* cmd_name, const char* usage, 
			int objc_min=0, int objc_max=-1, bool exact_objc=false);

  /// Virtual destructor ensures proper destruction of subclasses.
  virtual ~TclCmd();

  /// The procedure that is actually registered with the Tcl C API.
  static int dummyInvoke(ClientData clientData, Tcl_Interp* interp,
								 int objc, Tcl_Obj *const objv[]);

protected:
  /** This is overridden by subclasses to implement the specific
      functionality for the command that is represented. */
  virtual void invoke() = 0;

  /** Appends an appropriate warning about the command's proper usage
      to the Tcl interpreter's result, and sets the interpreter's
      result to \c TCL_ERROR. */
  virtual void usage();

  /// Append \a msg as an error message to the Tcl interpreter's result.
  void errorMessage(const char* msg);


  //---------------------------------------------------------------------
  //
  // Functions to query the arguments of the current invocation.
  //
  //---------------------------------------------------------------------

  /// Return the number of arguments in the current invocation.
  int objc() { return itsObjc; }

  /// Returns the argument number \a argn of the current invocation.
  TclValue& arg(int argn);

  /// Attempts to retrieve an \c int from argument number \a argn.
  int getIntFromArg(int argn);

  /// Attempts to retrieve a \c long from argument number \a argn.
  long getLongFromArg(int argn);

  /// Attempts to retrieve a \c bool from argument number \a argn.
  bool getBoolFromArg(int argn);

  /// Attempts to retrieve a \c double from argument number \a argn.
  double getDoubleFromArg(int argn);

  /// Attempts to retrieve a C-style string (\c char*) from argument number \a argn.
  const char* getCstringFromArg(int argn);

  /** Attempts to retrieve an string type from argument number \a
      argn. The templated type must be assignable from const char*. */
  template <class Str>
  Str getStringTypeFromArg(int argn, Str* /* dummy */ = 0)
	 {
		return Str(getCstringFromArg(argn));
	 }

  /** Attempt to convert argument number \a argn to type \c T, and
      copy the result into \a val. */
  template <class T>
  void getValFromArg(int argn, T& val)
	 { getValFromObj(itsObjv[argn], val); }

  /** Attempts to convert argument number \a argn into a sequence of
      elements of type \c T, and inserts these through the insert
      iterator \a itr. */
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

  /** Attempts to convert argument number \a argn into a sequence of
      values, and inserts these through the insert iterator \a
      itr. The iterator must come from a sequence of TclValue's. */
  template <class Iterator>
  void getValSequenceFromArg(int argn, Iterator itr) {
	 Tcl_Obj** elements;
	 int count;
	 if ( Tcl_ListObjGetElements(itsInterp, itsObjv[argn], &count, &elements)
			!= TCL_OK) {
		throw TclError();
	 }
	 for (int i = 0; i < count; ++i) {
		*itr = TclValue(itsInterp, elements[i]);
	 }
  }


  //---------------------------------------------------------------------
  //
  // Functions to return a value from the command
  //
  //---------------------------------------------------------------------

  /// Return satisfactorily with a void result.
  void returnVoid();

  /// Return an error condition with a void result.
  void returnError();

  /// Return satisfactorily with the \c int result \a val.
  void returnInt(int val);

  /// Return satisfactorily with the \c long result \a val.
  void returnLong(long val);

  /// Return satisfactorily with the \c bool result \a val.
  void returnBool(bool val);

  /// Return satisfactorily with the \c double result \a val.
  void returnDouble(double val);

  /// Return satisfactorily with the C-style string (\c char*) result \a val.
  void returnCstring(const char* val);

  /// Return satisfactorily with the \c Value result \a val.
  void returnValue(const Value& val);

  /** Return satisfactorily with the string type result \a val. The
      templated type must have a c_str() function returning const char*. */
  template <class Str>
  void returnStringType(const Str& val)
	 {
		returnCstring(val.c_str());
	 }

  /** Return satisfactorily with the generic type result \a val. The
      default behavior is to attempt to convert \a val to a const
      char* with a \c c_str() function, but specializations for the
      basic builtin types override this default. */
  template <class T>
  void returnVal(const T& val)
	 {
		returnVal(val.c_str());
	 }
  
  /** Append to the result a list element with generic type and the
      value \a val. The default version of \c lappendVal attempts to
      convert the value to const char* with a c_str() function, but
      specializations for the basic builtin types override this
      default. */
  template <class T>
  void lappendVal(const T& val)
	 {
		lappendVal(val.c_str());
	 }

  /// Return the sequence of values referred to by the range [\a begin, \a end).
  template <class Itr>
  void returnSequence(Itr begin, Itr end) {
	 while (begin != end) {
		lappendVal(*begin);
		++begin;
	 }
  }

private:
  void lappendValue(const Value& val);
  void lappendInt(int val);
  void lappendLong(long val);
  void lappendBool(bool val);
  void lappendDouble(double val);
  void lappendCstring(const char* val);

  template <class T>
  void getValFromObj(Tcl_Obj* obj, T& val);

  // These are set once per command object
  const char* const itsUsage;
  const int itsObjcMin;
  const int itsObjcMax;
  const bool itsExactObjc;

  // These are set once per invocation of the command object
  Tcl_Interp* itsInterp;
  int itsObjc;
  Tcl_Obj* const* itsObjv;

  class Impl; // this contains a vector<TclValue> to hold its args
  Impl* const itsImpl;

  int itsResult;
};

//---------------------------------------------------------------------
//
// Specializations of TclCmd::returnVal
//
//---------------------------------------------------------------------

template <> inline void Tcl::TclCmd::returnVal(const Value& val)
  { returnValue(val); }

template <> inline void Tcl::TclCmd::returnVal(int val)
  { returnInt(val); }

template <> inline void Tcl::TclCmd::returnVal(long val)
  { returnInt(val); }

template <> inline void Tcl::TclCmd::returnVal(bool val)
  { returnBool(val); }

template <> inline void Tcl::TclCmd::returnVal(double val)
  { returnDouble(val); }

template <> inline void Tcl::TclCmd::returnVal(const char* val)
  { returnCstring(val); }

//---------------------------------------------------------------------
//
// Specializations of TclCmd::lappendVal
//
//---------------------------------------------------------------------

template <> inline void Tcl::TclCmd::lappendVal(const Value& val)
  { lappendValue(val); }

template <> inline void Tcl::TclCmd::lappendVal(const int& val)
  { lappendInt(val); }

template <> inline void Tcl::TclCmd::lappendVal(const long& val)
  { lappendLong(val); }

template <> inline void Tcl::TclCmd::lappendVal(const bool& val)
  { lappendBool(val); }

template <> inline void Tcl::TclCmd::lappendVal(const double& val)
  { lappendDouble(val); }

template <> inline void Tcl::TclCmd::lappendVal(char* const& val)
  { lappendCstring(val); }

template <> inline void Tcl::TclCmd::lappendVal(const char* const& val)
  { lappendCstring(val); }

static const char vcid_tclcmd_h[] = "$Header$";
#endif // !TCLCMD_H_DEFINED
