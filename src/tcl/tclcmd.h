///////////////////////////////////////////////////////////////////////
//
// tclcmd.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Jun 11 14:50:43 1999
// written: Thu Jul 12 13:03:44 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLCMD_H_DEFINED
#define TCLCMD_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(POINTERS_H_DEFINED)
#include "util/pointers.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(CONVERT_H_DEFINED)
#include "tcl/convert.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TCLLISTOBJ_H_DEFINED)
#include "tcl/tcllistobj.h"
#endif

struct Tcl_Interp;
struct Tcl_Obj;
typedef void* ClientData;

class Value;

namespace Tcl
{
  class TclValue;
  class TclCmd;
  class Context;
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
 * \c TclCmd uses class \c Context to represent the set of Tcl command
 * arguments and the interpreter's result.
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

  /// Returns the command registered name.
  const char* name() const;

  /// Returns a string describing the command's proper usage.
  virtual const char* usage() const;

  /** This is overridden by subclasses to implement the specific
      functionality for the command that is represented. The \c
      Context& argument allows Tcl command arguments to be retrieved,
      and allows the interpreter's result to be set.*/
  virtual void invoke(Context& ctx) = 0;

  /** Makes \a other an overloaded version of \a this. Overloading is
      done by argument counts. The first TclCmd in an overload
      sequence to match the argument count of the context will be
      used. */
  void addOverload(Tcl_Interp* interp, shared_ptr<TclCmd> other);

protected:
  /** This may be overridden by subclasses that need to provide a
      different interface to the raw Tcl arguments (such as for
      vectorizing a function over a set of arguments). The default
      implementation just sets up a \c Context and calls \a invoke().
      Errors should be signaled by throwing appropriate exceptions,
      which will be caught and translated back to the Tcl interpreter
      by \a invokeCallback(). */
  virtual void rawInvoke(Tcl_Interp* interp, unsigned int objc,
                         Tcl_Obj* const objv[]);

private:
  TclCmd(const TclCmd&);
  TclCmd& operator=(const TclCmd&);

  class Impl;
  friend class Impl;
  Impl* const itsImpl;
};


///////////////////////////////////////////////////////////////////////
/**
 *
 * \c Tcl::Context, which is passed to \a TclCmd::invoke(), provides
 * an interface for getting values from command arguments (using
 * functions like \c getIntFromArg() and getCstringFromArg()), and an
 * interface for returning values to the Tcl interpreter's result
 * (using functions such as \c setResult() and \c restultAppender()).
 *
 **/
///////////////////////////////////////////////////////////////////////

class Tcl::Context {
public:
  friend class TclCmd;

  /// Construct with a Tcl interpreter and an argument list.
  Context(Tcl_Interp* interp, unsigned int objc, Tcl_Obj* const* objv);

  /// Virtual destructor.
  virtual ~Context();

  /// Get the Tcl interpreter of the current invocation.
  Tcl_Interp* interp() const { return itsInterp; }

  /// Return the number of arguments in the current invocation.
  unsigned int objc() { return itsObjc; }

  //---------------------------------------------------------------------
  //
  // Functions to query the arguments of the current invocation.
  //
  //---------------------------------------------------------------------

  /// Attempts to retrieve an \c int from argument number \a argn.
  int getIntFromArg(unsigned int argn)
    { return Tcl::Convert<int>::fromTcl(getObjv(argn)); }

  /// Attempts to retrieve a \c long from argument number \a argn.
  long getLongFromArg(unsigned int argn)
    { return Tcl::Convert<long>::fromTcl(getObjv(argn)); }

  /// Attempts to retrieve a \c bool from argument number \a argn.
  bool getBoolFromArg(unsigned int argn)
    { return Tcl::Convert<bool>::fromTcl(getObjv(argn)); }

  /// Attempts to retrieve a \c double from argument number \a argn.
  double getDoubleFromArg(unsigned int argn)
    { return Tcl::Convert<double>::fromTcl(getObjv(argn)); }

  /// Attempts to retrieve a C-style string (\c char*) from argument number \a argn.
  const char* getCstringFromArg(unsigned int argn)
    { return getStringFromArg(argn, (const char**) 0); }

  /** Attempts to retrieve an string type from argument number \a
      argn. The templated type must be assignable from const char*. */
  template <class Str>
  Str getStringFromArg(unsigned int argn, Str* /* dummy */ = 0)
    {
      return Str(Tcl::Convert<const char*>::fromTcl(getObjv(argn)));
    }

  /** Attempt to convert argument number \a argn to type \c T, and
      copy the result into \a val. */
  template <class T>
  T getValFromArg(unsigned int argn, T* /*dummy*/=0)
    {
      return Tcl::Convert<T>::fromTcl(getObjv(argn));
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
  void getSequenceFromArg(unsigned int argn, Iterator itr, T* /* dummy */)
    {
      Tcl::List elements(getObjv(argn));

      for (unsigned int i = 0; i < elements.length(); ++i)
        {
          *itr = Tcl::Convert<T>::fromTcl(elements[i]);
          ++itr;
        }
    }

  /** Attempts to convert argument number \a argn into a Tcl list, and
      if successful, returns an iterator pointing to the beginning of
      that list. */
  template <class T>
  List::Iterator<T> beginOfArg(unsigned int argn, T* /*dummy*/=0)
    {
      return List::Iterator<T>(getObjv(argn), List::Iterator<T>::BEGIN);
    }

  /** Attempts to convert argument number \a argn into a Tcl list, and
      if successful, returns an iterator pointing to the
      one-past-the-end element of that list. */
  template <class T>
  List::Iterator<T> endOfArg(unsigned int argn, T* /*dummy*/=0)
    {
      return List::Iterator<T>(getObjv(argn), List::Iterator<T>::END);
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
      setObjResult(Tcl::Convert<T>::toTcl(t));
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

    /// Destructor actually returns the value to the Context.
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
  /// Get the n'th argument.
  virtual Tcl_Obj* getObjv(unsigned int n) { return itsObjv[n]; }

  /// Return a Tcl_Obj*.
  virtual void setObjResult(Tcl_Obj* obj);

private:
  Context(const Context&);
  Context& operator=(const Context&);

  Tcl_Interp* const itsInterp;
  unsigned int const itsObjc;
  Tcl_Obj* const* const itsObjv;
};

static const char vcid_tclcmd_h[] = "$Header$";
#endif // !TCLCMD_H_DEFINED
