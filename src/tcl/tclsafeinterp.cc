///////////////////////////////////////////////////////////////////////
//
// tclsafeinterp.cc
//
// Copyright (c) 2000-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Wed Oct 11 10:27:35 2000
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

#ifndef TCLSAFEINTERP_CC_DEFINED
#define TCLSAFEINTERP_CC_DEFINED

#include "tcl/tclsafeinterp.h"

#include "util/demangle.h"
#include "util/error.h"
#include "util/strings.h"

#include <exception>
#include <tcl.h>
#include <typeinfo>

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER

namespace
{
  void interpDeleteProc(ClientData clientData, Tcl_Interp*)
  {
    Tcl::Interp* intp = static_cast<Tcl::Interp*>(clientData);
    intp->forgetInterp();
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
DOTRACE("Tcl::Interp::Interp");
  if (interp == 0)
    throw Util::Error("tried to make Tcl::Interp with a null Tcl_Interp*", SRC_POS);

  Tcl_CallWhenDeleted(itsInterp, interpDeleteProc,
                      static_cast<ClientData>(this));
}

Tcl::Interp::Interp(const Tcl::Interp& other) :
  itsInterp(other.itsInterp)
{
DOTRACE("Tcl::Interp::Interp(const Interp&)");

  if (itsInterp != 0)
    {
      Tcl_CallWhenDeleted(itsInterp, interpDeleteProc,
                          static_cast<ClientData>(this));
    }
}

Tcl::Interp::~Interp()
{
DOTRACE("Tcl::Interp::~Interp");

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
    throw Util::Error("Tcl::Interp doesn't have a valid interpreter", SRC_POS);

  return itsInterp;
}

bool Tcl::Interp::interpDeleted() const
{
DOTRACE("Tcl::Interp::interpDeleted");

  return !hasInterp() || bool(Tcl_InterpDeleted(intp()));
}

void Tcl::Interp::forgetInterp()
{
DOTRACE("Tcl::Interp::forgetInterp");
  itsInterp = 0;
}

void Tcl::Interp::destroy()
{
DOTRACE("Tcl::Interp::destroy");

  if (hasInterp())
    {
      Tcl_DeleteInterp(intp());
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
DOTRACE("Tcl::Interp::pkgProvide");
  Tcl_PkgProvide(intp(), name, version);
}

///////////////////////////////////////////////////////////////////////
//
// Tcl::Interp -- Expressions
//
///////////////////////////////////////////////////////////////////////

bool Tcl::Interp::evalBooleanExpr(const Tcl::ObjPtr& obj) const
{
DOTRACE("Tcl::Interp::evalBooleanExpr");

  int expr_result;

  if (Tcl_ExprBooleanObj(intp(), obj.obj(), &expr_result) != TCL_OK)
    {
      throw Util::Error("error evaluating boolean expression", SRC_POS);
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

bool Tcl::Interp::eval(const fstring& code, Tcl::ErrorStrategy strategy)
{
  Tcl::ObjPtr obj(Tcl::toTcl(code));
  return eval(obj, strategy);
}

bool Tcl::Interp::eval(const Tcl::ObjPtr& code, Tcl::ErrorStrategy strategy)
{
DOTRACE("Tcl::Interp::eval");

  if (!hasInterp())
    throw Util::Error("Tcl_Interp* was null in Tcl::Interp::eval", SRC_POS);

  if ( Tcl_EvalObjEx(intp(), code.obj(), TCL_EVAL_GLOBAL) == TCL_OK )
    return true;

  // else, there was some error during the Tcl eval...

  switch (strategy)
    {
    case THROW_ERROR:
      throw Util::Error(fstring("error while evaluating ",
                                Tcl_GetString(code.obj()),
                                ":\n", getResult<const char*>()),
                        SRC_POS);
      break;
    case IGNORE_ERROR:
      return false;
    }

  return false; // to indicate error
}

bool Tcl::Interp::evalFile(const char* fname)
{
DOTRACE("Tcl::Interp::evalFile");
  return (Tcl_EvalFile(intp(), fname) == TCL_OK);
}

void Tcl::Interp::sourceRCFile()
{
DOTRACE("Tcl::Interp::sourceRCFile");
  Tcl_SourceRCFile(intp());
}

///////////////////////////////////////////////////////////////////////
//
// Tcl::Interp -- Result
//
///////////////////////////////////////////////////////////////////////

void Tcl::Interp::resetResult() const
{
DOTRACE("Tcl::Interp::resetResult");

  Tcl_ResetResult(intp());
}

void Tcl::Interp::appendResult(const char* msg) const
{
DOTRACE("Tcl::Interp::appendResult(const char*)");

  Tcl_AppendResult(intp(), msg, (char*)0);
}

void Tcl::Interp::appendResult(const fstring& msg) const
{
DOTRACE("Tcl::Interp::appendResult(const fstring&)");

  Tcl_AppendResult(intp(), msg.c_str(), (char*)0);
}

Tcl_Obj* Tcl::Interp::getObjResult() const
{
DOTRACE("Tcl::Interp::getObjResult");

  return Tcl_GetObjResult(intp());
}

void Tcl::Interp::setObjResult(Tcl_Obj* obj)
{
DOTRACE("Tcl::Interp::setObjResult");

  Tcl_SetObjResult(intp(), obj);
}

///////////////////////////////////////////////////////////////////////
//
// Tcl::Interp -- Variables
//
///////////////////////////////////////////////////////////////////////

void Tcl::Interp::setGlobalVar(const char* var_name, const Tcl::ObjPtr& var) const
{
DOTRACE("Tcl::Interp::setGlobalVar");

  if (Tcl_SetVar2Ex(intp(), const_cast<char*>(var_name), /*name2*/0,
                    var.obj(), TCL_GLOBAL_ONLY) == 0)
    {
      throw Util::Error(fstring("couldn't set global variable'",
                                var_name, "'"), SRC_POS);
    }
}

void Tcl::Interp::unsetGlobalVar(const char* var_name) const
{
DOTRACE("Tcl::Interp::unsetGlobalVar");

  if (Tcl_UnsetVar(intp(), const_cast<char*>(var_name),
                   TCL_GLOBAL_ONLY) != TCL_OK)
    {
      throw Util::Error(fstring("couldn't unset global variable'",
                                var_name, "'"), SRC_POS);
    }
}

Tcl_Obj* Tcl::Interp::getObjGlobalVar(const char* name1,
                                      const char* name2) const
{
DOTRACE("Tcl::Interp::getObjGlobalVar");
  Tcl_Obj* obj = Tcl_GetVar2Ex(intp(),
                               const_cast<char*>(name1),
                               const_cast<char*>(name2),
                               TCL_GLOBAL_ONLY|TCL_LEAVE_ERR_MSG);

  if (obj == 0)
    {
      throw Util::Error(fstring("couldn't get global variable '",
                                name1, "'"), SRC_POS);
    }

  return obj;
}

void Tcl::Interp::linkInt(const char* varName, int* addr, bool readOnly)
{
DOTRACE("Tcl::Interp::linkInt");
  dbgEvalNL(3, varName);

  fstring temp = varName;

  int flag = TCL_LINK_INT;
  if (readOnly) flag |= TCL_LINK_READ_ONLY;

  if ( Tcl_LinkVar(intp(), temp.data(), reinterpret_cast<char *>(addr), flag)
       != TCL_OK )
    throw Util::Error("error while linking int variable", SRC_POS);
}

void Tcl::Interp::linkDouble(const char* varName, double* addr, bool readOnly)
{
DOTRACE("Tcl::Interp::linkDouble");
  dbgEvalNL(3, varName);

  fstring temp = varName;

  int flag = TCL_LINK_DOUBLE;
  if (readOnly) flag |= TCL_LINK_READ_ONLY;

  if ( Tcl_LinkVar(intp(), temp.data(), reinterpret_cast<char *>(addr), flag)
       != TCL_OK )
    throw Util::Error("error while linking double variable", SRC_POS);
}

void Tcl::Interp::linkBoolean(const char* varName, int* addr, bool readOnly)
{
DOTRACE("Tcl::Interp::linkBoolean");
  dbgEvalNL(3, varName);

  fstring temp = varName;

  int flag = TCL_LINK_BOOLEAN;
  if (readOnly) flag |= TCL_LINK_READ_ONLY;

  if ( Tcl_LinkVar(intp(), temp.data(), reinterpret_cast<char *>(addr), flag)
       != TCL_OK )
    throw Util::Error("error while linking boolean variable", SRC_POS);
}

void Tcl::Interp::handleLiveException(const char* where,
                                      bool withBkgdError) throw()
{
DOTRACE("Tcl::Interp::handleLiveException");

  try
    {
      throw;
    }
  catch (std::exception& err)
    {
      dbgPrintNL(3, "caught (std::exception&)");

      if (hasInterp())
        {
          fstring msg;

          if (where != 0 && where[0] != '\0')
            msg.append(where, ": ");

          const char* what = err.what();

          if (what != 0 && what[0] != '\0')
            msg.append(what, " ");

          msg.append("(", demangled_name(typeid(err)), ")");

          appendResult(msg);
        }
    }
  catch (...)
    {
      dbgPrintNL(3, "caught (...)");

      if (hasInterp())
        {
          fstring msg;

          if (where != 0 && where[0] != '\0')
            msg.append(where, ": ");

          msg.append("an error of unknown type occurred");

          appendResult(msg);
        }
    }

  if (withBkgdError)
    backgroundError();
}

void Tcl::Interp::backgroundError() throw()
{
DOTRACE("Tcl::Interp::backgroundError");

  if (hasInterp())
    Tcl_BackgroundError(itsInterp);
}

void Tcl::Interp::addErrorInfo(const char* info)
{
DOTRACE("Tcl::Interp::addErrorInfo");

  Tcl_AddErrorInfo(intp(), info);
}

void Tcl::Interp::clearEventQueue()
{
DOTRACE("Tcl::Interp::clearEventQueue");
  while (Tcl_DoOneEvent(TCL_ALL_EVENTS|TCL_DONT_WAIT) != 0)
    { /* Empty loop body */ }
}

bool Tcl::Interp::hasCommand(const char* cmd_name) const
{
DOTRACE("Tcl::Interp::hasCommand");
  Tcl_CmdInfo info;
  int result = Tcl_GetCommandInfo(intp(), cmd_name, &info);
  return (result != 0);
}

void Tcl::Interp::deleteCommand(const char* cmd_name)
{
DOTRACE("Tcl::Interp::deleteCommand");

  // We must check if the interp has been tagged for deletion already,
  // since if it is then we must not attempt to use it to delete a Tcl
  // command (this results in "called Tcl_HashEntry on deleted table"). Not
  // deleting the command in that case does not cause a resource leak,
  // however, since the Tcl_Interp as part if its own destruction will
  // delete all commands associated with it.
  if ( !interpDeleted() )
    {
      Tcl_DeleteCommand(intp(), cmd_name);
    }
}

fstring Tcl::Interp::getProcBody(const char* proc_name)
{
DOTRACE("Tcl::Interp::getProcBody");
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
DOTRACE("Tcl::Interp::createProc");

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
DOTRACE("Tcl::Interp::deleteProc");

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

static const char vcid_tclsafeinterp_cc[] = "$Header$";
#endif // !TCLSAFEINTERP_CC_DEFINED
