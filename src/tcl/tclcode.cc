///////////////////////////////////////////////////////////////////////
//
// tclcode.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jul 16 13:29:16 2001
// written: Wed Aug  8 15:59:41 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLCODE_CC_DEFINED
#define TCLCODE_CC_DEFINED

#include "tcl/tclcode.h"

#include "tcl/tclerror.h"
#include "tcl/tclsafeinterp.h"

#include "util/errorhandler.h"

#include <tcl.h>

class Tcl::Code::EvalError : public Tcl::TclError {
public:
  EvalError(Tcl_Obj* cmd_obj) :
    Tcl::TclError("error while evaluating ")
  {
    append(Tcl_GetString(cmd_obj));
  }

  EvalError(const char* msg) :
    Tcl::TclError(msg) {}

  FIX_COPY_CTOR(EvalError, Tcl::TclError);
};

Tcl::Code::Code() :
  itsCodeObj(),
  itsErrorMode(NONE),
  itsFlags(TCL_EVAL_GLOBAL),
  itsErrHandler()
{}

Tcl::Code::Code(Tcl::ObjPtr cmd, ErrorHandlingMode mode) :
  itsCodeObj(cmd),
  itsErrorMode(mode),
  itsFlags(TCL_EVAL_GLOBAL),
  itsErrHandler(0)
{}

Tcl::Code::Code(Tcl::ObjPtr cmd, Util::ErrorHandler* handler) :
  itsCodeObj(cmd),
  itsErrorMode(NONE),
  itsFlags(TCL_EVAL_GLOBAL),
  itsErrHandler(handler)
{}

bool Tcl::Code::invoke(Tcl_Interp* interp)
{
  if (interp == 0)
    throw EvalError("Tcl_Interp* was null in Tcl::Code::invoke");

  if ( Tcl_EvalObjEx(interp, itsCodeObj, itsFlags)
       != TCL_OK )
    {
      EvalError err(itsCodeObj);

      if (itsErrHandler != 0)
        {
          itsErrHandler->handleError(err);
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

bool Tcl::Code::invoke(const Tcl::Interp& interp)
{
  return invoke(interp.intp());
}

static const char vcid_tclcode_cc[] = "$Header$";
#endif // !TCLCODE_CC_DEFINED
