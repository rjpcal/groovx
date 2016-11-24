/** @file tcl/command.h binds a wrapped c++ function to a
    tcl::arg_spec and a tcl::dispatcher */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Fri Jun 11 14:50:43 1999
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
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

#ifndef GROOVX_TCL_COMMAND_H_UTC20050628162421_DEFINED
#define GROOVX_TCL_COMMAND_H_UTC20050628162421_DEFINED

#include "tcl/argspec.h"
#include "tcl/conversions.h"
#include "tcl/obj.h"

#include <memory>

typedef struct Tcl_Obj Tcl_Obj;

namespace rutz
{
  struct file_pos;
  class fstring;
}

namespace tcl
{
  class arg_spec;
  class function;
  class command;
  class call_context;
  class arg_dispatcher;
  class interpreter;
}

/// Abstract interface for the core function to be embedded in a tcl::command.
/** TODO replace with std::function<void(tcl::call_context&)> */
class tcl::function
{
public:
  virtual ~function() noexcept;

  /// Abstract function performs this command's specific functionality.
  /** The \c tcl::call_context& argument allows Tcl command arguments
      to be retrieved, and allows the interpreter's result to be
      set.*/
  virtual void invoke(tcl::call_context& ctx) = 0;
};

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c tcl::command provides a way to wrap Tcl commands in C++
 * classes. The \c tcl::command class itself takes care of such things
 * as checking the argument count, and issuing an error message if the
 * argument count is incorrect.
 *
 * \c tcl::command uses class \c tcl::call_context to represent the
 * set of Tcl command arguments and the interpreter's result.
 *
 * If more than one tcl::command is created with the same name, an
 * overloading sequence is created. Overloading is done by argument
 * counts. The first tcl::command in an overload sequence to match the
 * argument count of the context will be used.
 *
 * Most clients of tcl::command will be able to simply use
 * tcl::make_command() or tcl::make_vec_command(), which detect the
 * types of C++ functions and build generic tcl::command's that call
 * the functions appropriately, or use tcl::pkg::def() and related
 * functions, which call tcl::make_command() but in addition help to
 * relate the commands to a particular package.
 *
 **/
///////////////////////////////////////////////////////////////////////

class tcl::command
{
public:
  /// Build a tcl::command object.
  /** BUT, you almost certainly don't want to use this function
      directly, but should instead call tcl::command_group::make(). If
      you just create a tcl::command on its own, it won't do anything
      (it won't be registered with the tcl interpreter). The
      tcl::command needs to be hooked into a tcl::command_group, and
      the way to do that is by creating it through
      tcl::command_group::make(). */
  command(std::shared_ptr<tcl::function> callback,
          const char* usage, const arg_spec& spec);

  /// Non-virtual destructor since this class is not for use as a base class.
  ~command() noexcept;

  /// Returns a string describing the arguments expected by this command.
  rutz::fstring usage_string() const;

  /// Check if the given argument count is acceptable.
  bool allows_argc(unsigned int objc) const;

  /// Check if the given argument count is unacceptable.
  bool rejects_argc(unsigned int objc) const;

  /// Send arguments to its tcl::function via its tcl::arg_dispatcher.
  void call(tcl::interpreter& interp,
            unsigned int objc, Tcl_Obj* const objv[]);

  /// Get the current tcl::arg_dispatcher for this command.
  std::shared_ptr<arg_dispatcher> get_dispatcher() const;

  /// Change the tcl::arg_dispatcher for this command.
  void set_dispatcher(std::shared_ptr<arg_dispatcher> dpx);

private:
  command(const command&); // not implemented
  command& operator=(const command&); // not implemented

  class impl;
  impl* const rep;
};

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c tcl::arg_dispatcher. This may be subclassed in order to provide
 * a different interface to the raw Tcl arguments (such as for
 * vectorizing a function over a set of arguments). The default
 * dispatcher used by tcl::command's implementation just sets up a \c
 * tcl::call_context and calls \a invoke().  Errors should be signaled
 * by throwing appropriate exceptions, which will be caught and
 * returned back to the Tcl interpreter as normal Tcl error messages.
 *
 **/
///////////////////////////////////////////////////////////////////////

class tcl::arg_dispatcher
{
public:
  /// Virtual destructor.
  virtual ~arg_dispatcher() noexcept;

  /** Interprets the Tcl_Obj* arguments, sets up an appropriate
      tcl::call_context, and calls invoke() on the \a cmd with that
      context. */
  virtual void dispatch(tcl::interpreter& interp,
                        unsigned int objc, Tcl_Obj* const objv[],
                        tcl::function& callback) = 0;
};

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c tcl::call_context, which is passed to \a tcl::command::invoke(),
 * provides a get_arg() for getting values from command arguments, and
 * provides set_result() for returning values to the Tcl interpreter.
 *
 **/
///////////////////////////////////////////////////////////////////////

class tcl::call_context
{
public:
  friend class tcl::command;

  /// Construct with a Tcl interpreter and an argument list.
  call_context(tcl::interpreter& interp,
               unsigned int objc, Tcl_Obj* const* objv);

  /// Virtual destructor.
  virtual ~call_context() noexcept;

  /// Get the Tcl interpreter of the current invocation.
  tcl::interpreter& interp() const noexcept { return m_interp; }

  /// Return the number of arguments in the current invocation.
  unsigned int objc() const noexcept { return m_objc; }


  /** Attempt to convert argument number \a argn to type \c T, and
      return the result of the conversion. */
  template <class T>
  auto get_arg(unsigned int argn)
    {
      return tcl::convert_to<T>(get_objv(argn));
    }


  /// Return satisfactorily with the result \a t of type \c T.
  template <class T>
  void set_result(T&& t)
    {
      set_obj_result(tcl::convert_from(std::forward<T>(t)));
    }

  /// Get the raw objv array.
  Tcl_Obj* const* get_raw_args() const noexcept
    {
      return m_objv;
    }

protected:
  /// Get the n'th argument.
  virtual Tcl_Obj* get_objv(unsigned int n) noexcept { return m_objv[n]; }

  /// Return a Tcl_Obj*.
  virtual void set_obj_result(const tcl::obj& obj);

private:
  call_context(const call_context&);
  call_context& operator=(const call_context&);

  tcl::interpreter&        m_interp;
  unsigned int       const m_objc;
  Tcl_Obj* const*    const m_objv;
};

#endif // !GROOVX_TCL_COMMAND_H_UTC20050628162421_DEFINED
