///////////////////////////////////////////////////////////////////////
//
// tclcode.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jul 16 13:29:16 2001
// written: Thu Nov 21 12:23:01 2002
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

#include "util/trace.h"

class Tcl::Code::EvalError : public Tcl::TclError
{
public:
  EvalError(Tcl_Obj* cmd_obj, const char* result) :
    Tcl::TclError(fstring("error while evaluating ", Tcl_GetString(cmd_obj),
                          ":\n", result))
  {}

  EvalError(const char* msg) :
    Tcl::TclError(msg) {}
};

Tcl::Code::Code() :
  itsCodeObj(),
  itsErrorMode(IGNORE_ERRORS),
  itsFlags(TCL_EVAL_GLOBAL),
  itsErrHandler()
{}

Tcl::Code::Code(const Tcl::ObjPtr& cmd, ErrorHandlingMode mode) :
  itsCodeObj(cmd),
  itsErrorMode(mode),
  itsFlags(TCL_EVAL_GLOBAL),
  itsErrHandler(0)
{}

Tcl::Code::Code(const Tcl::ObjPtr& cmd, Util::ErrorHandler* handler) :
  itsCodeObj(cmd),
  itsErrorMode(IGNORE_ERRORS),
  itsFlags(TCL_EVAL_GLOBAL),
  itsErrHandler(handler)
{}

bool Tcl::Code::invoke(Tcl_Interp* interp)
{
DOTRACE("Tcl::Code::invoke");

  if (interp == 0)
    throw EvalError("Tcl_Interp* was null in Tcl::Code::invoke");

  if ( Tcl_EvalObjEx(interp, itsCodeObj.obj(), itsFlags) == TCL_OK )
    return true;

  // else, there was some error during the Tcl eval...

  if (IGNORE_ERRORS != itsErrorMode)
    {
      EvalError err(itsCodeObj.obj(), Tcl_GetStringResult(interp));

      if (itsErrHandler != 0)
        {
          itsErrHandler->handleError(err);
        }
      else if (THROW_EXCEPTION == itsErrorMode)
        {
          throw err;
        }
      else if (BACKGROUND_ERROR == itsErrorMode)
        {
          Tcl_AppendResult(interp, err.msg_cstr(), (char*) 0);
          Tcl_BackgroundError(interp);
        }
    }

  return false; // to indicate error
}

bool Tcl::Code::invoke(const Tcl::Interp& interp)
{
  return invoke(interp.intp());
}

static const char vcid_tclcode_cc[] = "$Header$";
#endif // !TCLCODE_CC_DEFINED
