///////////////////////////////////////////////////////////////////////
//
// tclcmd.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Jun 11 14:50:43 1999
// written: Sun Aug 26 08:35:11 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLCMD_H_DEFINED
#define TCLCMD_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TCLCONVERT_H_DEFINED)
#include "tcl/tclconvert.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TCLOBJPTR_H_DEFINED)
#include "tcl/tclobjptr.h"
#endif

struct Tcl_Interp;
struct Tcl_Obj;

namespace Tcl
{
  class TclCmd;
  class Context;
}

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c Tcl::TclCmd is an abstract class that which provides a way to
 * wrap Tcl object commands in C++ classes. It allows commands that do
 * similar things to be related through inheritance, and for common
 * behavior to be placed in base classes. The \c TclCmd class itself
 * takes care of such things as checking the argument count, and
 * issuing an error message if the argument count is incorrect.
 *
 * \c Tcl::TclCmd uses class \c Context to represent the set of Tcl
 * command arguments and the interpreter's result.
 *
 * If more than one Tcl::TclCmd is created with the same name, an
 * overloading sequence is created. Overloading is done by argument
 * counts. The first TclCmd in an overload sequence to match the
 * argument count of the context will be used.
 *
 * Most clients of Tcl::TclCmd will be able to simply use
 * Tcl::makeCmd() or Tcl::makeVecCmd(), which detect the types of C++
 * functions and build generic Tcl::TclCmd's that call the functions
 * appropriately, or use Tcl::Pkg::def() and related functions, which
 * call Tcl::makeCmd() but in addition help to relate the commands to
 * a particular package.
 *
 **/
///////////////////////////////////////////////////////////////////////

class Tcl::TclCmd
{
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
 * \c Tcl::Context, which is passed to \a TclCmd::invoke(), provides a
 * getValFromArg() for getting values from command arguments, and
 * provides setResult() for returning values to the Tcl interpreter.
 *
 **/
///////////////////////////////////////////////////////////////////////

class Tcl::Context
{
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

  //
  // To query the arguments of the current invocation:
  //

  /** Attempts to retrieve a C-style string (\c char*) from argument
      number \a argn. */
  const char* getCstringFromArg(unsigned int argn)
    { return getValFromArg(argn, TypeCue<const char*>()); }

  /** Attempt to convert argument number \a argn to type \c T, and
      copy the result into \a val. */
  template <class Cue>
  typename Return<typename Cue::Type>::Type
  getValFromArg(unsigned int argn, Cue)
    {
      return Tcl::Convert<typename Cue::Type>::fromTcl(getObjv(argn));
    }


  //
  // To return a value to the Tcl interpreter
  //

  /// Return satisfactorily with the result \a t of type \c T.
  template <class T>
  void setResult(T t)
    {
      setObjResult(Tcl::Convert<T>::toTcl(t));
    }

protected:
  /// Get the n'th argument.
  virtual Tcl_Obj* getObjv(unsigned int n) { return itsObjv[n]; }

  /// Return a Tcl_Obj*.
  virtual void setObjResult(Tcl::ObjPtr obj);

private:
  Context(const Context&);
  Context& operator=(const Context&);

  Tcl_Interp* const itsInterp;
  unsigned int const itsObjc;
  Tcl_Obj* const* const itsObjv;
};

static const char vcid_tclcmd_h[] = "$Header$";
#endif // !TCLCMD_H_DEFINED
