///////////////////////////////////////////////////////////////////////
//
// tclcmd.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Jun 11 14:50:43 1999
// written: Wed Jul 11 12:58:55 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLCMD_H_DEFINED
#define TCLCMD_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TRAITS_H_DEFINED)
#include "util/traits.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(CONVERT_H_DEFINED)
#include "tcl/convert.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TCLLISTOBJ_H_DEFINED)
#include "tcl/tcllistobj.h"
#endif

#ifdef HP9000S700
#define BROKEN_TEMPLATE_FRIEND
#endif

struct Tcl_Interp;
struct Tcl_Obj;
typedef void* ClientData;

class Value;

namespace Tcl
{
  class TclValue;
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
 * argument, as well as functions such as \c returnVal() and \c
 * returnString() for returning basic types to Tcl.
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

  /// Returns a string describing the command's proper usage.
  virtual const char* usage();

#ifndef BROKEN_TEMPLATE_FRIEND
protected:
#else
public:
#endif

  /** This is overridden by subclasses to implement the specific
      functionality for the command that is represented. */
  virtual void invoke() = 0;

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
  TclValue arg(int argn);

  /// Attempts to retrieve an \c int from argument number \a argn.
  int getIntFromArg(int argn) { return Tcl::fromTcl<int>(itsObjv[argn]); }

  /// Attempts to retrieve a \c long from argument number \a argn.
  long getLongFromArg(int argn) { return Tcl::fromTcl<long>(itsObjv[argn]); }

  /// Attempts to retrieve a \c bool from argument number \a argn.
  bool getBoolFromArg(int argn) { return Tcl::fromTcl<bool>(itsObjv[argn]); }

  /// Attempts to retrieve a \c double from argument number \a argn.
  double getDoubleFromArg(int argn) { return Tcl::fromTcl<double>(itsObjv[argn]); }

  /// Attempts to retrieve a C-style string (\c char*) from argument number \a argn.
  const char* getCstringFromArg(int argn) { return getStringFromArg(argn, (const char**) 0); }

  /** Attempts to retrieve an string type from argument number \a
      argn. The templated type must be assignable from const char*. */
  template <class Str>
  Str getStringFromArg(int argn, Str* /* dummy */ = 0)
    {
      return Str(Tcl::fromTcl<const char*>(itsObjv[argn]));
    }

  /** Attempt to convert argument number \a argn to type \c T, and
      copy the result into \a val. */
  template <class T>
  T getValFromArg(int argn, T* /*dummy*/=0)
    {
      return Tcl::fromTcl<T>(itsObjv[argn]);
    }

  //---------------------------------------------------------------------
  //
  // Functions to treat the arguments of the current invocation as Tcl
  // lists. There are two ways to handle arguments in such a way: one
  // is to transfer the whole list through an STL-style insert
  // iterator usinge getSequenceFromArg() , and the other is to use \c
  // Tcl::List::Iterator's from beginOfArg() and endOfArg(), which
  // iterate through a Tcl list and convert each value to a type
  // specified by the template argument of \c Tcl::List::Iterator.
  //
  //---------------------------------------------------------------------

  /** Attempts to convert argument number \a argn into a sequence of
      elements of type \c T, and inserts these through the insert
      iterator \a itr. */
  template <class T, class Iterator>
  void getSequenceFromArg(int argn, Iterator itr, T* /* dummy */) {
    Tcl::List elements(itsObjv[argn]);

    for (unsigned int i = 0; i < elements.length(); ++i) {
      *itr = Tcl::fromTcl<T>(elements[i]);
      ++itr;
    }
  }

  /** Attempts to convert argument number \a argn into a Tcl list, and
      if successful, returns an iterator pointing to the beginning of
      that list. */
  template <class T>
  List::Iterator<T> beginOfArg(int argn, T* /*dummy*/=0)
    {
      return List::Iterator<T>(itsObjv[argn], List::Iterator<T>::BEGIN);
    }


  /** Attempts to convert argument number \a argn into a Tcl list, and
      if successful, returns an iterator pointing to the
      one-past-the-end element of that list. */
  template <class T>
  List::Iterator<T> endOfArg(int argn, T* /*dummy*/=0)
    {
      return List::Iterator<T>(itsObjv[argn], List::Iterator<T>::END);
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

  /// Return satisfactorily with the result \a t of type \c T.
  template <class T>
  void returnVal(T t)
    {
      returnTclObj(Tcl::toTcl<T>(t));
    }

  /** Return satisfactorily with the string type result \a val. The
      templated type must have a c_str() function returning const char*. */
  template <class Str>
  void returnString(const Str& val)
    {
      returnVal(Util::StringTraits<Str>::c_str(val));
    }


  /// Append to the result a list element with the generic \c Value \a val.
  void lappendVal(const Value& val) { lappendValue(val); }

  /// Append to the result a list element with the \c unsigned \c char value \a val.
  void lappendVal(unsigned char val) { lappendInt(int(val)); }

  /// Append to the result a list element with the \c int value \a val.
  void lappendVal(int val) { lappendInt(val); }

  /// Append to the result a list element with the \c unsigned int value \a val.
  void lappendVal(unsigned int val) { lappendLong(long(val)); }

  /// Append to the result a list element with the \c long value \a val.
  void lappendVal(long val) { lappendLong(val); }

  /// Append to the result a list element with the \c unsigned long value \a val.
  void lappendVal(unsigned long val) { lappendLong(long(val)); }

  /// Append to the result a list element with the \c bool value \a val.
  void lappendVal(bool val) { lappendBool(val); }

  /// Append to the result a list element with the \c double value \a val.
  void lappendVal(double val) { lappendDouble(val); }

  /// Append to the result a list element with the C-style string (\c char*) value \a val.
  void lappendVal(const char* val) { lappendCstring(val); }

  /** Append to the result a list element with the string value \a
      val. The templated type must have a c_str() function returning
      const char*. */
  template <class Str>
  void lappendStringType(Str val)
    {
      lappendVal(Util::StringTraits<Str>::c_str(val));
    }

  /// Return the sequence of values referred to by the range [\a begin, \a end).
  template <class Itr>
  void returnSequence(Itr begin, Itr end) {
    while (begin != end) {
      lappendVal(*begin);
      ++begin;
    }
  }

  /** \c ResultAppender is an inserter (as well as an output iterator)
      that appends elements of type \c T to the result of the \c
      TclCmd with which it is initialized. */
  template <class T>
  class ResultAppender {
  public:
    /// Construct with a \c TclCmd whose result should be appended to.
    ResultAppender(TclCmd* aCmd) :
      itsCmd(aCmd) {}
    /// Copy constructor.
    ResultAppender(const ResultAppender& other) :
      itsCmd(other.itsCmd) {}
    /// Assignment operator.
    ResultAppender& operator=(const ResultAppender& other)
      { itsCmd = other.itsCmd; return *this; }

    /// Output assignment: \a val will be appended to the \c TclCmd's result.
    ResultAppender& operator=(const T& val)
      { itsCmd->lappendVal(val); return *this; }

    /// Dereference.
    ResultAppender& operator*() { return *this; }
    /// Pre-increment.
    ResultAppender& operator++() { return *this; }
    /// Post-increment.
    ResultAppender operator++(int) { return *this; }

  private:
    TclCmd* itsCmd;
  };

  template <class T> friend class ResultAppender;

  /// Return a \c ResultAppender of the given type for this \c TclCmd.
  template <class T>
  ResultAppender<T> resultAppender(T* /*dummy*/=0)
    { return ResultAppender<T>(this); }

protected:
  Tcl_Interp* interp() { return itsInterp; }

  /// Return a Tcl_Obj*.
  void returnTclObj(Tcl_Obj* obj);

private:
  /// The procedure that is actually registered with the Tcl C API.
  static int invokeCallback(ClientData clientData, Tcl_Interp* interp,
                            int objc, Tcl_Obj *const objv[]);

  int invokeTemplate();

  void lappendValue(const Value& val);
  void lappendInt(int val);
  void lappendLong(long val);
  void lappendBool(bool val);
  void lappendDouble(double val);
  void lappendCstring(const char* val);

  TclCmd(const TclCmd&);
  TclCmd& operator=(const TclCmd&);

  // These are set once per command object
  const char* const itsUsage;
  const int itsObjcMin;
  const int itsObjcMax;
  const bool itsExactObjc;

  // These are set once per invocation of the command object
  Tcl_Interp* itsInterp;
  int itsObjc;
  Tcl_Obj* const* itsObjv;

  class Impl;
  Impl* const itsImpl;

  int itsResult;
};


static const char vcid_tclcmd_h[] = "$Header$";
#endif // !TCLCMD_H_DEFINED
