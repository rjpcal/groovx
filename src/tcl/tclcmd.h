///////////////////////////////////////////////////////////////////////
//
// tclcmd.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Jun 11 14:50:43 1999
// written: Wed Jul 11 20:49:20 2001
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

#include "util/pointers.h"

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
 * \c TclCmd exposes a nested class \c Context, which is passed to \a
 * TclCmd::invoke() provides functions such as \c getIntFromArg() and
 * getCstringFromArg() for getting basic types from a command
 * argument, as well as functions such as \c setResult() for returning
 * basic types to Tcl.
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

  class Context
  {
  public:
    Context(Tcl_Interp* interp, int objc, Tcl_Obj* const* objv);

    Tcl_Interp* interp() const { return itsInterp; }

    /// Return the number of arguments in the current invocation.
    int objc() { return itsObjc; }

    //---------------------------------------------------------------------
    //
    // Functions to query the arguments of the current invocation.
    //
    //---------------------------------------------------------------------

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
    void getSequenceFromArg(int argn, Iterator itr, T* /* dummy */)
      {
        Tcl::List elements(itsObjv[argn]);

        for (unsigned int i = 0; i < elements.length(); ++i)
          {
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

    /// Return satisfactorily with the result \a t of type \c T.
    template <class T>
    void setResult(T t)
      {
        setObjResult(Tcl::toTcl<T>(t));
      }

    /// Return the sequence of values referred to by the range [\a begin, \a end).
    template <class Itr>
    void returnSequence(Itr begin, Itr end)
      {
        Tcl::List result;
        while (begin != end)
          {
            result.append(*begin);
            ++begin;
          }
        setResult(result);
      }

    /** \c ResultAppender is an inserter (as well as an output iterator)
      that appends elements of type \c T to the result of the \c
      TclCmd with which it is initialized. */
    template <class T>
    class ResultAppender {
    public:
      /// Construct with a \c TclCmd whose result should be appended to.
      ResultAppender(Context* ctx) :
        itsContext(ctx), itsList() {}

      /// Destructor actually returns the value to the TclCmd.
      ~ResultAppender()
      { itsContext->setResult(itsList); }

      /// Copy constructor.
      ResultAppender(const ResultAppender& other) :
        itsContext(other.itsContext), itsList(other.itsList) {}

      /// Assignment operator.
      ResultAppender& operator=(const ResultAppender& other)
      { itsContext = other.itsContext; itsList = other.itsList; return *this; }

      /// Output assignment: \a val will be appended to the \c TclCmd's result.
      ResultAppender& operator=(const T& val)
      { itsList.append(val); return *this; }

      /// Dereference.
      ResultAppender& operator*() { return *this; }
      /// Pre-increment.
      ResultAppender& operator++() { return *this; }
      /// Post-increment.
      ResultAppender operator++(int) { return *this; }

    private:
      Context* itsContext;
      Tcl::List itsList;
    };

    /// Return a \c ResultAppender of the given type for this \c TclCmd.
    template <class T>
    ResultAppender<T> resultAppender(T* /*dummy*/=0)
      { return ResultAppender<T>(this); }

  protected:
    /// Return a Tcl_Obj*.
    void setObjResult(Tcl_Obj* obj);

#if 0
  private:
#else
  public:
#endif
    Tcl_Interp* itsInterp;
    int itsObjc;
    Tcl_Obj* const* itsObjv;
    int itsResult;

  private:
    Context(const Context&);
    Context& operator=(const Context&);
  };


  /** This is overridden by subclasses to implement the specific
      functionality for the command that is represented. */
  virtual void invoke(Context& ctx) = 0;



private:
  /// The procedure that is actually registered with the Tcl C API.
  static int invokeCallback(ClientData clientData, Tcl_Interp* interp,
                            int objc, Tcl_Obj *const objv[]);

  TclCmd(const TclCmd&);
  TclCmd& operator=(const TclCmd&);

  // These are set once per command object
  const char* const itsUsage;
  const int itsObjcMin;
  const int itsObjcMax;
  const bool itsExactObjc;

//    // These are set once per invocation of the command object
//    Tcl_Interp* itsInterp;
//    int itsObjc;
//    Tcl_Obj* const* itsObjv;

  class Impl;
  Impl* const itsImpl;

  shared_ptr<Context> itsContext;

//    int itsResult;
};


///////////////////////////////////////////////////////////////////////
/**
 *
 * Tcl::TclCmd::Context definition
 *
 **/
///////////////////////////////////////////////////////////////////////

#if 0
class Tcl::TclCmd::Context {
};
#endif

static const char vcid_tclcmd_h[] = "$Header$";
#endif // !TCLCMD_H_DEFINED
