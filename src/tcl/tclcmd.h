///////////////////////////////////////////////////////////////////////
//
// tclcmd.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Jun 11 14:50:43 1999
// written: Wed Dec 18 08:40:32 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLCMD_H_DEFINED
#define TCLCMD_H_DEFINED

#include "tcl/tclconvert.h"
#include "tcl/tclobjptr.h"

struct Tcl_Obj;

class fstring;

template <class T> class shared_ptr;

namespace Tcl
{
  class Command;
  class Context;
  class Dispatcher;
  class Interp;
}

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c Tcl::Command is an abstract class that which provides a way to
 * wrap Tcl object commands in C++ classes. It allows commands that do
 * similar things to be related through inheritance, and for common
 * behavior to be placed in base classes. The \c Tcl::Command class
 * itself takes care of such things as checking the argument count,
 * and issuing an error message if the argument count is incorrect.
 *
 * \c Tcl::Command uses class \c Context to represent the set of Tcl
 * command arguments and the interpreter's result.
 *
 * If more than one Tcl::Command is created with the same name, an
 * overloading sequence is created. Overloading is done by argument
 * counts. The first Tcl::Command in an overload sequence to match the
 * argument count of the context will be used.
 *
 * Most clients of Tcl::Command will be able to simply use
 * Tcl::makeCmd() or Tcl::makeVecCmd(), which detect the types of C++
 * functions and build generic Tcl::Command's that call the functions
 * appropriately, or use Tcl::Pkg::def() and related functions, which
 * call Tcl::makeCmd() but in addition help to relate the commands to
 * a particular package.
 *
 **/
///////////////////////////////////////////////////////////////////////

class Tcl::Command
{
public:
  /** Construct with basic properties for the command. If \a
      exact_objc is true, then the \a objc of a command invocation is
      required to be exactly equal either \a objc_min or \a objc_max;
      if it is false, then \a objc must be between \a objc_min and \a
      objc_max, inclusive. If the value given for \a objc_max is
      negative, then the maximum objc will be set to the same value as
      \a objc_min. */
  Command(Tcl::Interp& interp, const char* cmd_name, const char* usage,
          int objc_min=0, int objc_max=-1, bool exact_objc=false);

  /// Virtual destructor ensures proper destruction of subclasses.
  virtual ~Command();

  /// Returns the command registered name.
  const fstring& name() const;

  /// Returns a string describing the command's proper usage, including overloads.
  fstring usage() const;

  /// Returns a string describing this single command's usage.
  fstring rawUsage() const;

  /// Check if the given argument count is acceptable.
  bool allowsObjc(unsigned int objc) const;

  /// Check if the given argument count is unacceptable.
  bool rejectsObjc(unsigned int objc) const;

  /// Abstract function performs this command's specific functionality.
  /** The \c Context& argument allows Tcl command arguments to be
      retrieved, and allows the interpreter's result to be set.*/
  virtual void invoke(Context& ctx) = 0;

  /// Raw invocation.
  int rawInvoke(int objc, Tcl_Obj* const objv[]);

  /// Get the current Tcl::Dispatcher for this command.
  shared_ptr<Dispatcher> getDispatcher() const;

  /// Change the Tcl::Dispatcher for this command.
  void setDispatcher(shared_ptr<Dispatcher> dpx);

  /// Find the command associated with a given name.
  /** Returns null if no such command. */
  static Tcl::Command* lookup(const char* name);

private:
  Command(const Command&);
  Command& operator=(const Command&);

  class Impl;
  Impl* const rep;
};

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c Tcl::Dispatcher. This may be subclassed in order to provide a
 * different interface to the raw Tcl arguments (such as for
 * vectorizing a function over a set of arguments). The default
 * dispatcher used by Tcl::Command's implementation just sets up a \c
 * Context and calls \a invoke().  Errors should be signaled by
 * throwing appropriate exceptions, which will be caught and
 * translated back to the Tcl interpreter by \a invokeCallback().
 *
 **/
///////////////////////////////////////////////////////////////////////

class Tcl::Dispatcher
{
public:
  /// Virtual destructor.
  virtual ~Dispatcher();

  /** Interprets the Tcl_Obj* arguments, sets up an appropriate
      Tcl::Context, and calls invoke() on the \a cmd with that
      context. */
  virtual void dispatch(Tcl::Interp& interp,
                        unsigned int objc, Tcl_Obj* const objv[],
                        Tcl::Command& cmd) = 0;
};

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c Tcl::Context, which is passed to \a Tcl::Command::invoke(),
 * provides a getValFromArg() for getting values from command
 * arguments, and provides setResult() for returning values to the Tcl
 * interpreter.
 *
 **/
///////////////////////////////////////////////////////////////////////

class Tcl::Context
{
public:
  friend class Command;

  /// Construct with a Tcl interpreter and an argument list.
  Context(Tcl::Interp& interp, unsigned int objc, Tcl_Obj* const* objv);

  /// Virtual destructor.
  virtual ~Context();

  /// Get the Tcl interpreter of the current invocation.
  Tcl::Interp& interp() const { return itsInterp; }

  /// Return the number of arguments in the current invocation.
  unsigned int objc() const { return itsObjc; }


  /** Attempt to convert argument number \a argn to type \c T, and
      copy the result into \a val. */
  template <class T>
  typename Return<T>::Type getValFromArg(unsigned int argn)
    {
      return Tcl::toNative<T>(getObjv(argn));
    }


  /// Return satisfactorily with the result \a t of type \c T.
  template <class T>
  void setResult(T t)
    {
      setObjResult(Tcl::toTcl(t));
    }

  /// Get the raw objv array.
  Tcl_Obj* const* getRawArgs() const
    {
      return itsObjv;
    }

protected:
  /// Get the n'th argument.
  virtual Tcl_Obj* getObjv(unsigned int n) { return itsObjv[n]; }

  /// Return a Tcl_Obj*.
  virtual void setObjResult(const Tcl::ObjPtr& obj);

private:
  Context(const Context&);
  Context& operator=(const Context&);

  Tcl::Interp& itsInterp;
  unsigned int const itsObjc;
  Tcl_Obj* const* const itsObjv;
};

static const char vcid_tclcmd_h[] = "$Header$";
#endif // !TCLCMD_H_DEFINED
