///////////////////////////////////////////////////////////////////////
//
// tclsafeinterp.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Oct 11 10:27:35 2000
// written: Mon Sep 17 11:31:48 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLSAFEINTERP_CC_DEFINED
#define TCLSAFEINTERP_CC_DEFINED

#include "tcl/tclsafeinterp.h"

#include "tcl/tclcode.h"
#include "tcl/tclerror.h"

#include "util/strings.h"

#include <tcl.h>

#include "util/trace.h"
#include "util/debug.h"

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
    throw Tcl::TclError("tried to make Tcl::Interp with a null Tcl_Interp*");

  Tcl_CallWhenDeleted(itsInterp, interpDeleteProc,
                      static_cast<ClientData>(this));
}

Tcl::Interp::~Interp()
{
  if (itsInterp != 0)
    Tcl_DontCallWhenDeleted(itsInterp, interpDeleteProc,
                            static_cast<ClientData>(this));
}

Tcl_Interp* Tcl::Interp::intp() const
{
  if (itsInterp == 0)
    throw Tcl::TclError("Tcl::Interp doesn't have a valid interpreter");

  return itsInterp;
}

void Tcl::Interp::forgetInterp()
{
  itsInterp = 0;
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
      handleError("error evaluating boolean expression");
    }

  return bool(expr_result);
}

///////////////////////////////////////////////////////////////////////
//
// Tcl::Interp -- Interpreter
//
///////////////////////////////////////////////////////////////////////

bool Tcl::Interp::interpDeleted() const
{
DOTRACE("Tcl::Interp::interpDeleted");

  return bool(Tcl_InterpDeleted(intp()));
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
DOTRACE("Tcl::Interp::appendResult");

  Tcl_AppendResult(intp(), msg, (char*)0);
}

Tcl_Obj* Tcl::Interp::getObjResult() const
{
DOTRACE("Tcl::Interp::getObjResult");

  return Tcl_GetObjResult(intp());
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
      handleError("couldn't set global variable");
    }
}

void Tcl::Interp::unsetGlobalVar(const char* var_name) const
{
DOTRACE("Tcl::Interp::unsetGlobalVar");

  if (Tcl_UnsetVar(intp(), const_cast<char*>(var_name),
                   TCL_GLOBAL_ONLY) != TCL_OK)
    {
      handleError("couldn't unset global variable");
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
      handleError("couldn't get the Tcl variable");
    }

  return obj;
}

void Tcl::Interp::handleLiveException(const char* where,
                                      bool withBkgdError) throw()
{
DOTRACE("Tcl::Interp::handleLiveException");

  try
    {
      throw;
    }
  catch (Util::Error& err)
    {
      if (hasInterp())
        {
          Tcl_AppendResult(itsInterp, where, ": ", err.msg_cstr(), (char*) 0);
        }
    }
  catch (...)
    {
      if (hasInterp())
        {
          Tcl_AppendResult(itsInterp, where, ": ",
                           "an error of unknown type occurred", (char*) 0);
        }
    }

  if (withBkgdError && hasInterp())
    backgroundError();
}

void Tcl::Interp::backgroundError() throw()
{
DOTRACE("Tcl::Interp::backgroundError");

  if (hasInterp())
    Tcl_BackgroundError(itsInterp);
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
  int result = Tcl_GetCommandInfo(intp(),
                                  const_cast<char*>(cmd_name),
                                  &info);
  return (result != 0);
}

fstring Tcl::Interp::getProcBody(const char* proc_name) const
{
DOTRACE("Tcl::Interp::getProcBody");
  if (hasCommand(proc_name))
    {
      resetResult();

      Tcl::ObjPtr cmd_obj("info body ");
      cmd_obj.append(proc_name);

      Tcl::Code cmd(cmd_obj, Tcl::Code::NONE);

      if (cmd.invoke(intp()))
        {
          fstring result = getResult(TypeCue<const char*>());
          resetResult();
          return result;
        }
      else
        {
          throw Tcl::TclError(fstring("couldn't get the proc body for '",
                                      proc_name, "'"));
        }
    }

  return "";
}

void Tcl::Interp::createProc(const char* namesp, const char* proc_name,
                             const char* args, const char* body)
{
DOTRACE("Tcl::Interp::createProc");
  Tcl::ObjPtr proc_cmd_str;
  if (namesp && (*namesp != '\0'))
    {
      proc_cmd_str.append("namespace eval ");
      proc_cmd_str.append(namesp);
      proc_cmd_str.append(" { ");
    }
  proc_cmd_str.append("proc ");
  proc_cmd_str.append(proc_name);
  proc_cmd_str.append(" {");
  if (args)
    proc_cmd_str.append(args);
  proc_cmd_str.append("} {");
  proc_cmd_str.append(body);
  proc_cmd_str.append("} ");
  if (namesp)
    proc_cmd_str.append(" }");

  Tcl::Code proc_cmd(proc_cmd_str, Tcl::Code::THROW_EXCEPTION);
  proc_cmd.invoke(intp());
}

void Tcl::Interp::handleError(const char* msg) const
{
DOTRACE("Tcl::Interp::handleError");

  throw TclError(msg);
}

static const char vcid_tclsafeinterp_cc[] = "$Header$";
#endif // !TCLSAFEINTERP_CC_DEFINED
