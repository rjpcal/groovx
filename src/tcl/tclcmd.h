///////////////////////////////////////////////////////////////////////
//
// tclcmd.h
//
// Copyright (c) 1999-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Fri Jun 11 14:50:43 1999
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLCMD_H_DEFINED
#define TCLCMD_H_DEFINED

#include "tcl/tclconvert.h"
#include "tcl/tclobjptr.h"

#include "util/sharedptr.h"

typedef struct Tcl_Obj Tcl_Obj;

namespace rutz
{
  class file_pos;
  class fstring;
  template <class T> class shared_ptr;
}

namespace Tcl
{
  class Callback;
  class Command;
  class Context;
  class Dispatcher;
  class Interp;
}

class Tcl::Callback
{
public:
  virtual ~Callback() throw();

  /// Abstract function performs this command's specific functionality.
  /** The \c Context& argument allows Tcl command arguments to be
      retrieved, and allows the interpreter's result to be set.*/
  virtual void invoke(Context& ctx) = 0;
};

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
  /// Construct with basic properties for the command.
  /** If \a exact_objc is true, then the \a objc of a command
      invocation is required to be exactly equal either \a objc_min or
      \a objc_max; if it is false, then \a objc must be between \a
      objc_min and \a objc_max, inclusive. If the value given for \a
      objc_max is negative, then the maximum objc will be set to the
      same value as \a objc_min. */
  static rutz::shared_ptr<Command>
  make(Tcl::Interp& interp,
       rutz::shared_ptr<Tcl::Callback> callback,
       const char* cmd_name,
       const char* usage,
       int objc_min,
       int objc_max,
       bool exact_objc,
       const rutz::file_pos& src_pos);

  /// Virtual destructor ensures proper destruction of subclasses.
  ~Command() throw();

  /// Returns a string describing the arguments expected by this command.
  rutz::fstring usageString() const;

  /// Check if the given argument count is acceptable.
  bool allowsObjc(unsigned int objc) const;

  /// Check if the given argument count is unacceptable.
  bool rejectsObjc(unsigned int objc) const;

  /// Send arguments to its Tcl::Callback via its Tcl::Dispatcher.
  void call(Tcl::Interp& interp,
            unsigned int objc, Tcl_Obj* const objv[]);

  /// Get the current Tcl::Dispatcher for this command.
  rutz::shared_ptr<Dispatcher> getDispatcher() const;

  /// Change the Tcl::Dispatcher for this command.
  void setDispatcher(rutz::shared_ptr<Dispatcher> dpx);

private:
  Command(rutz::shared_ptr<Tcl::Callback> callback,
          const char* usage,
          int objc_min=0, int objc_max=-1, bool exact_objc=false);

  Command(const Command&); // not implemented
  Command& operator=(const Command&); // not implemented

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
  virtual ~Dispatcher() throw();

  /** Interprets the Tcl_Obj* arguments, sets up an appropriate
      Tcl::Context, and calls invoke() on the \a cmd with that
      context. */
  virtual void dispatch(Tcl::Interp& interp,
                        unsigned int objc, Tcl_Obj* const objv[],
                        Tcl::Callback& callback) = 0;
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
  virtual ~Context() throw();

  /// Get the Tcl interpreter of the current invocation.
  Tcl::Interp& interp() const throw() { return itsInterp; }

  /// Return the number of arguments in the current invocation.
  unsigned int objc() const throw() { return itsObjc; }


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
  Tcl_Obj* const* getRawArgs() const throw()
    {
      return itsObjv;
    }

protected:
  /// Get the n'th argument.
  virtual Tcl_Obj* getObjv(unsigned int n) throw() { return itsObjv[n]; }

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
