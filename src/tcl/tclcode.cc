///////////////////////////////////////////////////////////////////////
//
// tclcode.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jul 16 13:29:16 2001
// written: Mon Jul 16 14:20:10 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLCODE_CC_DEFINED
#define TCLCODE_CC_DEFINED

#include "tcl/tclcode.h"

#include "tcl/tclerror.h"
#include "tcl/tclutil.h"

#include "util/errorhandler.h"

#include <tcl.h>

class Tcl::Code::EvalError : public Tcl::TclError {
public:
  EvalError(Tcl_Obj* cmd_obj) :
    Tcl::TclError("error while evaluating ")
  {
    appendMsg(Tcl_GetString(cmd_obj));
  }
  EvalError(const char* msg) :
    Tcl::TclError(msg) {}
};

Tcl::Code::Code() :
  itsCodeObj(),
  itsErrorMode(NONE),
  itsFlags(TCL_EVAL_GLOBAL)
{}

Tcl::Code::Code(const char* tcl_cmd, ErrorHandlingMode mode) :
  itsCodeObj(Tcl_NewStringObj(tcl_cmd, -1)),
  itsErrorMode(mode),
  itsFlags(TCL_EVAL_GLOBAL),
  itsErrorHandler(0)
{}

Tcl::Code::Code(Tcl_Obj* cmd_object, ErrorHandlingMode mode) :
  itsCodeObj(cmd_object),
  itsErrorMode(mode),
  itsFlags(TCL_EVAL_GLOBAL),
  itsErrorHandler(0)
{}

Tcl::Code::Code(Tcl_Obj* cmd_object, Util::ErrorHander* handler) :
  itsCodeObj(cmd_object),
  itsErrorMode(NONE),
  itsFlags(TCL_EVAL_GLOBAL),
  itsErrorHandler(handler)
{}

bool Tcl::Code::invoke(Tcl_Interp* interp)
{
  if (interp == 0)
    throw EvalError("Tcl_Interp* was null in Tcl::Code::invoke");

  if ( Tcl_EvalObjEx(interp, itsCodeObj, itsFlags)
       != TCL_OK )
    {
      EvalError err(itsCodeObj);

      if (itsErrorHandler != 0)
        {
          itsErrorHandler->handleErrorWithMsg(err);
        }
      else if (NONE == itsErrorMode)
        {
          return false;
        }
      else if (THROW_EXCEPTION == itsErrorMode)
        {
          throw err;
        }
      else if (BACKGROUND_ERROR == itsErrorMode)
        {
          Tcl_AppendResult(interp, err.msg_cstr(), (char*) 0);
          Tcl_BackgroundError(interp);
          return false;
        }
    }

  return true;
}

bool Tcl::Code::invoke(Tcl::SafeInterp& interp)
{
  invoke(interp.intp());
}

static const char vcid_tclcode_cc[] = "$Header$";
#endif // !TCLCODE_CC_DEFINED
