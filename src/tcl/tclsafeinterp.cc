///////////////////////////////////////////////////////////////////////
//
// tclsafeinterp.cc
//
// Copyright (c) 2000-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Wed Oct 11 10:27:35 2000
// commit: $Id$
// $HeadURL$
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

#ifndef GROOVX_TCL_TCLSAFEINTERP_CC_UTC20050626084017_DEFINED
#define GROOVX_TCL_TCLSAFEINTERP_CC_UTC20050626084017_DEFINED

#include "tcl/tclsafeinterp.h"

#include "rutz/demangle.h"
#include "rutz/error.h"
#include "rutz/fstring.h"

#include "tcl/tcllistobj.h" // for evalObjv()

#include <exception>
#include <tcl.h>
#include <typeinfo>

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

using rutz::fstring;

namespace
{
  void interpDeleteProc(ClientData clientData, Tcl_Interp*) throw()
  {
    Tcl::Interp* intp = static_cast<Tcl::Interp*>(clientData);
    intp->forgetInterp();
  }

  bool reportError(Tcl::Interp& interp, const Tcl::ObjPtr& code,
                   Tcl::ErrorStrategy strategy,
                   const rutz::file_pos& where)
  {
    switch (strategy)
      {
      case Tcl::THROW_ERROR:
        {
          const fstring msg("error while evaluating ",
                            Tcl_GetString(code.obj()),
                            ":\n", interp.getResult<const char*>());

          // Now clear the interpreter's result string, since we've
          // already incorporated that message into our error message:
          interp.resetResult();

          throw rutz::error(msg, where);
        }
        break;
      case Tcl::IGNORE_ERROR:
        return false;
      }

    return false;
  }
}

///////////////////////////////////////////////////////////////////////
//
// Tcl::Interp member definitions
//
///////////////////////////////////////////////////////////////////////

Tcl::Interp::Interp(Tcl_Interp* interp) :
  itsInterp(interp)
{
GVX_TRACE("Tcl::Interp::Interp");
  if (interp == 0)
    throw rutz::error("tried to make Tcl::Interp "
                      "with a null Tcl_Interp*", SRC_POS);

  Tcl_CallWhenDeleted(itsInterp, interpDeleteProc,
                      static_cast<ClientData>(this));
}

Tcl::Interp::Interp(const Tcl::Interp& other) throw() :
  itsInterp(other.itsInterp)
{
GVX_TRACE("Tcl::Interp::Interp(const Interp&)");

  if (itsInterp != 0)
    {
      Tcl_CallWhenDeleted(itsInterp, interpDeleteProc,
                          static_cast<ClientData>(this));
    }
}

Tcl::Interp::~Interp() throw()
{
GVX_TRACE("Tcl::Interp::~Interp");

  if (itsInterp != 0)
    Tcl_DontCallWhenDeleted(itsInterp, interpDeleteProc,
                            static_cast<ClientData>(this));
}

///////////////////////////////////////////////////////////////////////
//
// Tcl::Interp -- Interpreter
//
///////////////////////////////////////////////////////////////////////

Tcl_Interp* Tcl::Interp::intp() const
{
  if (itsInterp == 0)
    throw rutz::error("Tcl::Interp doesn't have a valid interpreter",
                      SRC_POS);

  return itsInterp;
}

bool Tcl::Interp::interpDeleted() const throw()
{
GVX_TRACE("Tcl::Interp::interpDeleted");

  return (itsInterp == 0) || Tcl_InterpDeleted(itsInterp);
}

void Tcl::Interp::forgetInterp() throw()
{
GVX_TRACE("Tcl::Interp::forgetInterp");
  itsInterp = 0;
}

void Tcl::Interp::destroy() throw()
{
GVX_TRACE("Tcl::Interp::destroy");

  if (itsInterp != 0)
    {
      Tcl_DeleteInterp(itsInterp);
      itsInterp = 0;
    }
}

///////////////////////////////////////////////////////////////////////
//
// Tcl::Interp -- Packages
//
///////////////////////////////////////////////////////////////////////

void Tcl::Interp::pkgProvide(const char* name, const char* version)
{
GVX_TRACE("Tcl::Interp::pkgProvide");
  Tcl_PkgProvide(intp(), name, version);
}

///////////////////////////////////////////////////////////////////////
//
// Tcl::Interp -- Expressions
//
///////////////////////////////////////////////////////////////////////

bool Tcl::Interp::evalBooleanExpr(const Tcl::ObjPtr& obj) const
{
GVX_TRACE("Tcl::Interp::evalBooleanExpr");

  int expr_result;

  if (Tcl_ExprBooleanObj(intp(), obj.obj(), &expr_result) != TCL_OK)
    {
      throw rutz::error("error evaluating boolean expression", SRC_POS);
    }

  return bool(expr_result);
}

///////////////////////////////////////////////////////////////////////
//
// Tcl::Interp -- Evaluating code
//
///////////////////////////////////////////////////////////////////////

bool Tcl::Interp::eval(const char* code, Tcl::ErrorStrategy strategy)
{
  Tcl::ObjPtr obj(Tcl::toTcl(code));
  return eval(obj, strategy);
}

bool Tcl::Interp::eval(const fstring& code,
                       Tcl::ErrorStrategy strategy)
{
  Tcl::ObjPtr obj(Tcl::toTcl(code));
  return eval(obj, strategy);
}

bool Tcl::Interp::eval(const Tcl::ObjPtr& code,
                       Tcl::ErrorStrategy strategy)
{
GVX_TRACE("Tcl::Interp::eval");

  if (!hasInterp())
    throw rutz::error("Tcl_Interp* was null "
                      "in Tcl::Interp::eval", SRC_POS);

  // We want to use TCL_EVAL_DIRECT here because that will avoid a
  // string conversion cycle -- that may be important if we have
  // objects with fragile representations (i.e., objects that can't
  // survive a object->string->object cycle because their string
  // representations don't represent the full object value).

  if ( Tcl_EvalObjEx(intp(), code.obj(),
                     TCL_EVAL_DIRECT | TCL_EVAL_GLOBAL) == TCL_OK )
    return true;

  // else, there was some error during the Tcl eval...

  return reportError(*this, code, strategy, SRC_POS);
}

bool Tcl::Interp::evalObjv(const Tcl::List& objv,
                           Tcl::ErrorStrategy strategy)
{
GVX_TRACE("Tcl::Interp::evalObjv");

  if (!this->hasInterp())
    throw rutz::error("Tcl_Interp* was null "
                      "in Tcl::Interp::eval", SRC_POS);

  if ( Tcl_EvalObjv(this->intp(), objv.length(), objv.elements(),
                    TCL_EVAL_GLOBAL) == TCL_OK )
    return true;

  // else, there was some error during the Tcl eval...

  return reportError(*this, objv.asObj(), strategy, SRC_POS);
}

bool Tcl::Interp::evalFile(const char* fname)
{
GVX_TRACE("Tcl::Interp::evalFile");
  return (Tcl_EvalFile(intp(), fname) == TCL_OK);
}

void Tcl::Interp::sourceRCFile()
{
GVX_TRACE("Tcl::Interp::sourceRCFile");
  Tcl_SourceRCFile(intp());
}

///////////////////////////////////////////////////////////////////////
//
// Tcl::Interp -- Result
//
///////////////////////////////////////////////////////////////////////

void Tcl::Interp::resetResult() const
{
GVX_TRACE("Tcl::Interp::resetResult");

  Tcl_ResetResult(intp());
}

void Tcl::Interp::appendResult(const char* msg) const
{
GVX_TRACE("Tcl::Interp::appendResult(const char*)");

  Tcl_AppendResult(intp(), msg, static_cast<char*>(0));
}

void Tcl::Interp::appendResult(const fstring& msg) const
{
GVX_TRACE("Tcl::Interp::appendResult(const fstring&)");

  Tcl_AppendResult(intp(), msg.c_str(), static_cast<char*>(0));
}

Tcl_Obj* Tcl::Interp::getObjResult() const
{
GVX_TRACE("Tcl::Interp::getObjResult");

  return Tcl_GetObjResult(intp());
}

void Tcl::Interp::setObjResult(Tcl_Obj* obj)
{
GVX_TRACE("Tcl::Interp::setObjResult");

  Tcl_SetObjResult(intp(), obj);
}

///////////////////////////////////////////////////////////////////////
//
// Tcl::Interp -- Variables
//
///////////////////////////////////////////////////////////////////////

void Tcl::Interp::setGlobalVar(const char* var_name,
                               const Tcl::ObjPtr& var) const
{
GVX_TRACE("Tcl::Interp::setGlobalVar");

  if (Tcl_SetVar2Ex(intp(), const_cast<char*>(var_name), /*name2*/0,
                    var.obj(), TCL_GLOBAL_ONLY) == 0)
    {
      throw rutz::error(fstring("couldn't set global variable'",
                                var_name, "'"), SRC_POS);
    }
}

void Tcl::Interp::unsetGlobalVar(const char* var_name) const
{
GVX_TRACE("Tcl::Interp::unsetGlobalVar");

  if (Tcl_UnsetVar(intp(), const_cast<char*>(var_name),
                   TCL_GLOBAL_ONLY) != TCL_OK)
    {
      throw rutz::error(fstring("couldn't unset global variable'",
                                var_name, "'"), SRC_POS);
    }
}

Tcl_Obj* Tcl::Interp::getObjGlobalVar(const char* name1,
                                      const char* name2) const
{
GVX_TRACE("Tcl::Interp::getObjGlobalVar");
  Tcl_Obj* obj = Tcl_GetVar2Ex(intp(),
                               const_cast<char*>(name1),
                               const_cast<char*>(name2),
                               TCL_GLOBAL_ONLY|TCL_LEAVE_ERR_MSG);

  if (obj == 0)
    {
      throw rutz::error(fstring("couldn't get global variable '",
                                name1, "'"), SRC_POS);
    }

  return obj;
}

void Tcl::Interp::linkInt(const char* varName, int* addr,
                          bool readOnly)
{
GVX_TRACE("Tcl::Interp::linkInt");
  dbg_eval_nl(3, varName);

  fstring temp = varName;

  int flag = TCL_LINK_INT;
  if (readOnly) flag |= TCL_LINK_READ_ONLY;

  if ( Tcl_LinkVar(intp(), temp.data(),
                   reinterpret_cast<char *>(addr), flag)
       != TCL_OK )
    throw rutz::error("error while linking int variable", SRC_POS);
}

void Tcl::Interp::linkDouble(const char* varName, double* addr,
                             bool readOnly)
{
GVX_TRACE("Tcl::Interp::linkDouble");
  dbg_eval_nl(3, varName);

  fstring temp = varName;

  int flag = TCL_LINK_DOUBLE;
  if (readOnly) flag |= TCL_LINK_READ_ONLY;

  if ( Tcl_LinkVar(intp(), temp.data(),
                   reinterpret_cast<char *>(addr), flag)
       != TCL_OK )
    throw rutz::error("error while linking double variable", SRC_POS);
}

void Tcl::Interp::linkBoolean(const char* varName, int* addr,
                              bool readOnly)
{
GVX_TRACE("Tcl::Interp::linkBoolean");
  dbg_eval_nl(3, varName);

  fstring temp = varName;

  int flag = TCL_LINK_BOOLEAN;
  if (readOnly) flag |= TCL_LINK_READ_ONLY;

  if ( Tcl_LinkVar(intp(), temp.data(),
                   reinterpret_cast<char *>(addr), flag)
       != TCL_OK )
    throw rutz::error("error while linking boolean variable", SRC_POS);
}

void Tcl::Interp::handleLiveException(const char* where,
                                      const rutz::file_pos& pos) throw()
{
GVX_TRACE("Tcl::Interp::handleLiveException");

  try
    {
      throw;
    }
  catch (std::exception& err)
    {
      dbg_print_nl(3, "caught (std::exception&)");

      if (hasInterp())
        {
          fstring msg;

          msg.append(rutz::demangled_name(typeid(err)), " caught at ",
                     pos.m_file_name, ":", pos.m_line_no, ":\n");

          if (where != 0 && where[0] != '\0')
            msg.append(where, ": ");

          const char* what = err.what();

          if (what != 0 && what[0] != '\0')
            msg.append(what, " ");

          appendResult(msg);
        }
    }
  catch (...)
    {
      dbg_print_nl(3, "caught (...)");

      if (hasInterp())
        {
          fstring msg;

          msg.append("exception of unknown type caught at ",
                     pos.m_file_name, ":", pos.m_line_no, ":\n");

          if (where != 0 && where[0] != '\0')
            msg.append(where, ": ");

          appendResult(msg);
        }
    }
}

void Tcl::Interp::backgroundError() throw()
{
GVX_TRACE("Tcl::Interp::backgroundError");

  if (hasInterp())
    Tcl_BackgroundError(itsInterp);
}

void Tcl::Interp::addErrorInfo(const char* info)
{
GVX_TRACE("Tcl::Interp::addErrorInfo");

  Tcl_AddErrorInfo(intp(), info);
}

void Tcl::Interp::clearEventQueue()
{
GVX_TRACE("Tcl::Interp::clearEventQueue");
  while (Tcl_DoOneEvent(TCL_ALL_EVENTS|TCL_DONT_WAIT) != 0)
    { /* Empty loop body */ }
}

bool Tcl::Interp::hasCommand(const char* cmd_name) const
{
GVX_TRACE("Tcl::Interp::hasCommand");
  Tcl_CmdInfo info;
  int result = Tcl_GetCommandInfo(intp(), cmd_name, &info);
  return (result != 0);
}

void Tcl::Interp::deleteCommand(const char* cmd_name)
{
GVX_TRACE("Tcl::Interp::deleteCommand");

  // We must check if the interp has been tagged for deletion already,
  // since if it is then we must not attempt to use it to delete a Tcl
  // command (this results in "called Tcl_HashEntry on deleted
  // table"). Not deleting the command in that case does not cause a
  // resource leak, however, since the Tcl_Interp as part if its own
  // destruction will delete all commands associated with it.
  if ( !interpDeleted() )
    {
      Tcl_DeleteCommand(intp(), cmd_name);
    }
}

fstring Tcl::Interp::getProcBody(const char* proc_name)
{
GVX_TRACE("Tcl::Interp::getProcBody");
  if (hasCommand(proc_name))
    {
      resetResult();

      if (eval(fstring("info body ", proc_name)))
        {
          fstring result = getResult<const char*>();
          resetResult();
          return result;
        }
    }

  return "";
}

void Tcl::Interp::createProc(const char* namesp, const char* proc_name,
                             const char* args, const char* body)
{
GVX_TRACE("Tcl::Interp::createProc");

  if (namesp == 0 || (*namesp == '\0'))
    {
      namesp = "::";
    }

  fstring proc_cmd;
  proc_cmd.append("namespace eval ", namesp);
  proc_cmd.append(" { proc ", proc_name, " {");
  if (args)
    proc_cmd.append(args);
  proc_cmd.append("} {", body, "} }");

  eval(proc_cmd);
}

void Tcl::Interp::deleteProc(const char* namesp, const char* proc_name)
{
GVX_TRACE("Tcl::Interp::deleteProc");

  fstring cmd_str;

  cmd_str.append("rename ");

  if (namesp && (*namesp != '\0'))
    {
      cmd_str.append(namesp);
    }

  // by renaming to the empty string "", we delete the Tcl proc
  cmd_str.append("::", proc_name, " \"\"");

  eval(cmd_str);
}

static const char vcid_groovx_tcl_tclsafeinterp_cc_utc20050626084017[] = "$Id$ $HeadURL$";
#endif // !GROOVX_TCL_TCLSAFEINTERP_CC_UTC20050626084017_DEFINED
