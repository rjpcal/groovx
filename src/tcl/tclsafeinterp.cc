///////////////////////////////////////////////////////////////////////
//
// tclutil.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Oct 11 10:27:35 2000
// written: Mon Jul 16 14:58:23 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLUTIL_CC_DEFINED
#define TCLUTIL_CC_DEFINED

#include "tcl/tclutil.h"

#include <tcl.h>

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// Tcl::SafeInterp member definitions
//
///////////////////////////////////////////////////////////////////////

Tcl::SafeInterp::SafeInterp(Tcl_Interp* interp) :
  itsInterp(interp)
{
DOTRACE("Tcl::SafeInterp::SafeInterp");
  Invariant(itsInterp != 0);
}

Tcl::SafeInterp::~SafeInterp() {}

///////////////////////////////////////////////////////////////////////
//
// Tcl::SafeInterp -- Expressions
//
///////////////////////////////////////////////////////////////////////

bool Tcl::SafeInterp::evalBooleanExpr(Tcl_Obj* obj) const {
DOTRACE("Tcl::SafeInterp::evalBooleanExpr");

  int expr_result;

  if (Tcl_ExprBooleanObj(itsInterp, obj, &expr_result) != TCL_OK)
    {
      handleError("error evaluating boolean expression");
    }

  return bool(expr_result);
}

///////////////////////////////////////////////////////////////////////
//
// Tcl::SafeInterp -- Interpreter
//
///////////////////////////////////////////////////////////////////////

bool Tcl::SafeInterp::interpDeleted() const {
DOTRACE("Tcl::SafeInterp::interpDeleted");

  Invariant(itsInterp != 0);
  return bool(Tcl_InterpDeleted(itsInterp));
}

///////////////////////////////////////////////////////////////////////
//
// Tcl::SafeInterp -- Result
//
///////////////////////////////////////////////////////////////////////

void Tcl::SafeInterp::resetResult() const {
DOTRACE("Tcl::SafeInterp::resetResult");

  Tcl_ResetResult(itsInterp);
}

void Tcl::SafeInterp::appendResult(const char* msg) const {
DOTRACE("Tcl::SafeInterp::appendResult");

  Tcl_AppendResult(itsInterp, msg, (char*)0);
}

Tcl_Obj* Tcl::SafeInterp::getObjResult() const {
DOTRACE("Tcl::SafeInterp::getObjResult");

  return Tcl_GetObjResult(itsInterp);
}

///////////////////////////////////////////////////////////////////////
//
// Tcl::SafeInterp -- Variables
//
///////////////////////////////////////////////////////////////////////

void Tcl::SafeInterp::setGlobalVar(const char* var_name,
                                   Tcl::ObjPtr var) const
{
DOTRACE("Tcl::SafeInterp::setGlobalVar");

  if (Tcl_SetVar2Ex(itsInterp, const_cast<char*>(var_name), /*name2*/0,
                    var, TCL_GLOBAL_ONLY) == 0)
    {
      handleError("couldn't set global variable");
    }
}

void Tcl::SafeInterp::unsetGlobalVar(const char* var_name) const
{
DOTRACE("Tcl::SafeInterp::unsetGlobalVar");

  if (Tcl_UnsetVar(itsInterp, const_cast<char*>(var_name),
                   TCL_GLOBAL_ONLY) != TCL_OK)
    {
      handleError("couldn't unset global variable");
    }
}

Tcl_Obj* Tcl::SafeInterp::getObjGlobalVar(const char* name1,
                                          const char* name2) const
{
DOTRACE("Tcl::SafeInterp::getObjGlobalVar");
  Tcl_Obj* obj = Tcl_GetVar2Ex(itsInterp,
                               const_cast<char*>(name1),
                               const_cast<char*>(name2),
                               TCL_GLOBAL_ONLY|TCL_LEAVE_ERR_MSG);

  if (obj == 0)
    {
      handleError("couldn't get the Tcl variable");
    }

  return obj;
}

void Tcl::SafeInterp::clearEventQueue() {
DOTRACE("Tcl::SafeInterp::clearEventQueue");
  while (Tcl_DoOneEvent(TCL_ALL_EVENTS|TCL_DONT_WAIT) != 0)
    { /* Empty loop body */ }
}

bool Tcl::SafeInterp::hasCommand(const char* cmd_name) const {
DOTRACE("Tcl::SafeInterp::hasCommand");
  Tcl_CmdInfo info;
  int result = Tcl_GetCommandInfo(itsInterp,
                                  const_cast<char*>(cmd_name),
                                  &info);
  return (result != 0);
}

void Tcl::SafeInterp::handleError(const char* msg) const {
DOTRACE("Tcl::SafeInterp::handleError");

  throw TclError(msg);
}

static const char vcid_tclutil_cc[] = "$Header$";
#endif // !TCLUTIL_CC_DEFINED
