/** @file tcl/interp.cc c++ wrapper for Tcl_Interp, translates between
    tcl error codes and c++ exceptions */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Wed Oct 11 10:27:35 2000
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [https://github.com/rjpcal/groovx]
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

#include "tcl/interp.h"

#include "rutz/demangle.h"
#include "rutz/error.h"
#include "rutz/fstring.h"
#include "rutz/sfmt.h"

#include "tcl/list.h" // for eval_objv()

#include <exception>
#include <tcl.h>
#include <typeinfo>

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

using rutz::fstring;

namespace
{
  void c_interp_delete_proc(void* clientdata, Tcl_Interp*) noexcept
  {
    tcl::interpreter* intp = static_cast<tcl::interpreter*>(clientdata);
    intp->forget_interp();
  }

  bool report_error(tcl::interpreter& interp, const tcl::obj& code,
                    tcl::error_strategy strategy,
                    const rutz::file_pos& where)
  {
    switch (strategy)
      {
      case tcl::error_strategy::THROW:
        {
          const fstring msg =
            rutz::sfmt("error while evaluating %s:\n%s",
                       Tcl_GetString(code.get()),
                       interp.get_result<const char*>());

          // Now clear the interpreter's result string, since we've
          // already incorporated that message into our error message:
          interp.reset_result();

          throw rutz::error(msg, where);
        }

      case tcl::error_strategy::IGNORE:
        return false;
      }

    return false;
  }
}

///////////////////////////////////////////////////////////////////////
//
// tcl::interpreter member definitions
//
///////////////////////////////////////////////////////////////////////

tcl::interpreter::interpreter(Tcl_Interp* interp) :
  m_interp(interp)
{
GVX_TRACE("tcl::interpreter::interpreter");
  if (interp == nullptr)
    throw rutz::error("tried to make tcl::interpreter "
                      "with a null Tcl_Interp*", SRC_POS);

  Tcl_CallWhenDeleted(m_interp, c_interp_delete_proc,
                      static_cast<void*>(this));
}

tcl::interpreter::interpreter(const tcl::interpreter& other) noexcept :
  m_interp(other.m_interp)
{
GVX_TRACE("tcl::interpreter::interpreter(const interpreter&)");

  if (m_interp != nullptr)
    {
      Tcl_CallWhenDeleted(m_interp, c_interp_delete_proc,
                          static_cast<void*>(this));
    }
}

tcl::interpreter::~interpreter() noexcept
{
GVX_TRACE("tcl::interpreter::~interpreter");

  if (m_interp != nullptr)
    Tcl_DontCallWhenDeleted(m_interp, c_interp_delete_proc,
                            static_cast<void*>(this));
}

///////////////////////////////////////////////////////////////////////
//
// tcl::interpreter -- Tcl_Interp management
//
///////////////////////////////////////////////////////////////////////

Tcl_Interp* tcl::interpreter::intp() const
{
  if (m_interp == nullptr)
    throw rutz::error("tcl::interpreter doesn't have a valid Tcl_Interp*",
                      SRC_POS);

  return m_interp;
}

bool tcl::interpreter::is_deleted() const noexcept
{
GVX_TRACE("tcl::interpreter::is_deleted");

  return (m_interp == nullptr) || Tcl_InterpDeleted(m_interp);
}

void tcl::interpreter::forget_interp() noexcept
{
GVX_TRACE("tcl::interpreter::forget_interp");
  m_interp = nullptr;
}

void tcl::interpreter::destroy() noexcept
{
GVX_TRACE("tcl::interpreter::destroy");

  if (m_interp != nullptr)
    {
      Tcl_DeleteInterp(m_interp);
      m_interp = nullptr;
    }
}

///////////////////////////////////////////////////////////////////////
//
// tcl::interpreter -- Packages
//
///////////////////////////////////////////////////////////////////////

void tcl::interpreter::pkg_provide(const char* name, const char* version)
{
GVX_TRACE("tcl::interpreter::pkg_provide");
  Tcl_PkgProvide(intp(), name, version);
}

///////////////////////////////////////////////////////////////////////
//
// tcl::interpreter -- Expressions
//
///////////////////////////////////////////////////////////////////////

bool tcl::interpreter::eval_boolean_expr(const tcl::obj& obj) const
{
GVX_TRACE("tcl::interpreter::eval_boolean_expr");

  int expr_result;

  if (Tcl_ExprBooleanObj(intp(), obj.get(), &expr_result) != TCL_OK)
    {
      throw rutz::error("error evaluating boolean expression", SRC_POS);
    }

  return bool(expr_result);
}

///////////////////////////////////////////////////////////////////////
//
// tcl::interpreter -- Evaluating code
//
///////////////////////////////////////////////////////////////////////

bool tcl::interpreter::eval(const char* code,
                            tcl::error_strategy strategy)
{
  tcl::obj obj(tcl::convert_from(code));
  return eval(obj, strategy);
}

bool tcl::interpreter::eval(const fstring& code,
                            tcl::error_strategy strategy)
{
  tcl::obj obj(tcl::convert_from(code));
  return eval(obj, strategy);
}

bool tcl::interpreter::eval(const tcl::obj& code,
                            tcl::error_strategy strategy)
{
GVX_TRACE("tcl::interpreter::eval");

  if (!is_valid())
    throw rutz::error("Tcl_Interp* was null "
                      "in tcl::interpreter::eval", SRC_POS);

  // We want to use TCL_EVAL_DIRECT here because that will avoid a
  // string conversion cycle -- that may be important if we have
  // objects with fragile representations (i.e., objects that can't
  // survive a object->string->object cycle because their string
  // representations don't represent the full object value).

  if ( Tcl_EvalObjEx(intp(), code.get(),
                     TCL_EVAL_DIRECT | TCL_EVAL_GLOBAL) == TCL_OK )
    return true;

  // else, there was some error during the Tcl eval...

  return report_error(*this, code, strategy, SRC_POS);
}

bool tcl::interpreter::eval_objv(const tcl::list& objv,
                                tcl::error_strategy strategy)
{
GVX_TRACE("tcl::interpreter::eval_objv");

  if (!this->is_valid())
    throw rutz::error("Tcl_Interp* was null "
                      "in tcl::interpreter::eval", SRC_POS);

  if ( Tcl_EvalObjv(this->intp(), int(objv.length()), objv.elements(),
                    TCL_EVAL_GLOBAL) == TCL_OK )
    return true;

  // else, there was some error during the Tcl eval...

  return report_error(*this, objv.as_obj(), strategy, SRC_POS);
}

bool tcl::interpreter::eval_file(const char* fname)
{
GVX_TRACE("tcl::interpreter::eval_file");
  return (Tcl_EvalFile(intp(), fname) == TCL_OK);
}

void tcl::interpreter::source_rc_file()
{
GVX_TRACE("tcl::interpreter::source_rc_file");
  Tcl_SourceRCFile(intp());
}

///////////////////////////////////////////////////////////////////////
//
// tcl::interpreter -- Result
//
///////////////////////////////////////////////////////////////////////

void tcl::interpreter::reset_result() const
{
GVX_TRACE("tcl::interpreter::reset_result");

  Tcl_ResetResult(intp());
}

void tcl::interpreter::append_result(const char* msg) const
{
GVX_TRACE("tcl::interpreter::append_result(const char*)");

  Tcl_AppendResult(intp(), msg, static_cast<char*>(0));
}

void tcl::interpreter::append_result(const fstring& msg) const
{
GVX_TRACE("tcl::interpreter::append_result(const fstring&)");

  Tcl_AppendResult(intp(), msg.c_str(), static_cast<char*>(0));
}

Tcl_Obj* tcl::interpreter::get_obj_result() const
{
GVX_TRACE("tcl::interpreter::get_obj_result");

  return Tcl_GetObjResult(intp());
}

void tcl::interpreter::set_obj_result(Tcl_Obj* obj)
{
GVX_TRACE("tcl::interpreter::set_obj_result");

  Tcl_SetObjResult(intp(), obj);
}

///////////////////////////////////////////////////////////////////////
//
// tcl::interpreter -- Variables
//
///////////////////////////////////////////////////////////////////////

void tcl::interpreter::set_global_var(const char* var_name,
                                    const tcl::obj& var) const
{
GVX_TRACE("tcl::interpreter::set_global_var");

  if (Tcl_SetVar2Ex(intp(), const_cast<char*>(var_name), /*name2*/0,
                    var.get(), TCL_GLOBAL_ONLY) == 0)
    {
      throw rutz::error(rutz::sfmt("couldn't set global variable '%s'",
                                   var_name), SRC_POS);
    }
}

void tcl::interpreter::unset_global_var(const char* var_name) const
{
GVX_TRACE("tcl::interpreter::unset_global_var");

  if (Tcl_UnsetVar(intp(), const_cast<char*>(var_name),
                   TCL_GLOBAL_ONLY) != TCL_OK)
    {
      throw rutz::error(rutz::sfmt("couldn't unset global variable '%s'",
                                   var_name), SRC_POS);
    }
}

Tcl_Obj* tcl::interpreter::get_obj_global_var(const char* name1,
                                              const char* name2) const
{
GVX_TRACE("tcl::interpreter::get_obj_global_var");
  Tcl_Obj* obj = Tcl_GetVar2Ex(intp(),
                               const_cast<char*>(name1),
                               const_cast<char*>(name2),
                               TCL_GLOBAL_ONLY|TCL_LEAVE_ERR_MSG);

  if (obj == nullptr)
    {
      throw rutz::error(rutz::sfmt("couldn't get global variable '%s'",
                                   name1), SRC_POS);
    }

  return obj;
}

void tcl::interpreter::link_int(const char* var_name, int* addr,
                                bool read_only)
{
GVX_TRACE("tcl::interpreter::link_int");
  dbg_eval_nl(3, var_name);

  int flag = TCL_LINK_INT;
  if (read_only) flag |= TCL_LINK_READ_ONLY;

  if ( Tcl_LinkVar(intp(), var_name,
                   reinterpret_cast<char *>(addr), flag)
       != TCL_OK )
    throw rutz::error("error while linking int variable", SRC_POS);
}

void tcl::interpreter::link_double(const char* var_name, double* addr,
                                   bool read_only)
{
GVX_TRACE("tcl::interpreter::link_double");
  dbg_eval_nl(3, var_name);

  int flag = TCL_LINK_DOUBLE;
  if (read_only) flag |= TCL_LINK_READ_ONLY;

  if ( Tcl_LinkVar(intp(), var_name,
                   reinterpret_cast<char *>(addr), flag)
       != TCL_OK )
    throw rutz::error("error while linking double variable", SRC_POS);
}

void tcl::interpreter::link_boolean(const char* var_name, int* addr,
                                    bool read_only)
{
GVX_TRACE("tcl::interpreter::link_boolean");
  dbg_eval_nl(3, var_name);

  int flag = TCL_LINK_BOOLEAN;
  if (read_only) flag |= TCL_LINK_READ_ONLY;

  if ( Tcl_LinkVar(intp(), var_name,
                   reinterpret_cast<char *>(addr), flag)
       != TCL_OK )
    throw rutz::error("error while linking boolean variable", SRC_POS);
}

void tcl::interpreter::handle_live_exception(const char* where,
                                             const rutz::file_pos& pos) noexcept
{
GVX_TRACE("tcl::interpreter::handle_live_exception");

  try
    {
      throw;
    }
  catch (std::exception& err)
    {
      dbg_print_nl(3, "caught (std::exception&)");

      if (is_valid())
        {
          const char* what = err.what();

          const fstring msg =
            rutz::sfmt("%s caught at %s:%d:\n%s%s",
                       rutz::demangled_name(typeid(err)),
                       pos.m_file_name, pos.m_line_no,
                       ((where != nullptr && where[0] != '\0')
                        ? rutz::sfmt("%s: ", where).c_str()
                        : ""),
                       ((what != nullptr && what[0] != '\0')
                        ? rutz::sfmt("%s ", what).c_str()
                        : ""));

          append_result(msg);
        }
    }
  catch (...)
    {
      dbg_print_nl(3, "caught (...)");

      if (is_valid())
        {
          const fstring msg =
            rutz::sfmt("exception of unknown type caught at %s:%d\n%s",
                       pos.m_file_name, pos.m_line_no,
                       ((where != nullptr && where[0] != '\0')
                        ? where
                        : ""));

          append_result(msg);
        }
    }
}

void tcl::interpreter::background_error() noexcept
{
GVX_TRACE("tcl::interpreter::background_error");

  if (is_valid())
    Tcl_BackgroundError(m_interp);
}

void tcl::interpreter::add_error_info(const char* info)
{
GVX_TRACE("tcl::interpreter::add_error_info");

  Tcl_AddErrorInfo(intp(), info);
}

void tcl::interpreter::clear_event_queue()
{
GVX_TRACE("tcl::interpreter::clear_event_queue");
  while (Tcl_DoOneEvent(TCL_ALL_EVENTS|TCL_DONT_WAIT) != 0)
    { /* Empty loop body */ }
}

bool tcl::interpreter::has_command(const char* cmd_name) const
{
GVX_TRACE("tcl::interpreter::has_command");
  Tcl_CmdInfo info;
  int result = Tcl_GetCommandInfo(intp(), cmd_name, &info);
  return (result != 0);
}

void tcl::interpreter::delete_command(const char* cmd_name)
{
GVX_TRACE("tcl::interpreter::delete_command");

  // We must check if the interp has been tagged for deletion already,
  // since if it is then we must not attempt to use it to delete a Tcl
  // command (this results in "called Tcl_HashEntry on deleted
  // table"). Not deleting the command in that case does not cause a
  // resource leak, however, since the Tcl_Interp as part if its own
  // destruction will delete all commands associated with it.
  if ( !is_deleted() )
    {
      Tcl_DeleteCommand(intp(), cmd_name);
    }
}

fstring tcl::interpreter::get_proc_body(const char* proc_name)
{
GVX_TRACE("tcl::interpreter::get_proc_body");
  if (has_command(proc_name))
    {
      reset_result();

      if (eval(rutz::sfmt("info body %s", proc_name)))
        {
          fstring result = get_result<const char*>();
          reset_result();
          return result;
        }
    }

  return "";
}

void tcl::interpreter::create_proc(const char* namesp, const char* proc_name,
                                  const char* args, const char* body)
{
GVX_TRACE("tcl::interpreter::create_proc");

  if (namesp == nullptr || (*namesp == '\0'))
    {
      namesp = "::";
    }

  const fstring proc_cmd =
    rutz::sfmt("namespace eval %s { proc %s {%s} {%s} }",
               namesp, proc_name, args ? args : "", body);

  eval(proc_cmd);
}

void tcl::interpreter::delete_proc(const char* namesp, const char* proc_name)
{
GVX_TRACE("tcl::interpreter::delete_proc");

  // by renaming to the empty string "", we delete the Tcl proc
  const fstring cmd_str =
    rutz::sfmt("rename %s::%s \"\"",
               ((namesp != nullptr) && (*namesp != '\0')) ? namesp : "",
               proc_name);

  eval(cmd_str);
}
