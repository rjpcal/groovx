///////////////////////////////////////////////////////////////////////
//
// tclevalcmd.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Jun 17 10:38:13 1999
// written: Wed Mar 29 14:07:32 2000
// $Id$
//
// TclEvalCmd serves as a wrapper for a Tcl command string that is to
// be TclEval'd. It creates an object from the string, locks the
// object to ensure that it is not garbage collected, and provides a
// function to invoke the command in a Tcl_Interp. The command is
// byte-compiled by Tcl upon its first invocation.
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLEVALCMD_H_DEFINED
#define TCLEVALCMD_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TCLOBJLOCK_H_DEFINED)
#include "tcl/tclobjlock.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TCLERROR_H_DEFINED)
#include "tcl/tclerror.h"
#endif

namespace Tcl {
  class TclEvalCmd;
}

class Tcl::TclEvalCmd {
public:
  class EvalError : public Tcl::TclError {
  public:
	 EvalError(Tcl_Obj* cmd_obj) :
		Tcl::TclError("error while evaluating ")
		{
		  appendMsg(Tcl_GetString(cmd_obj));
		}
  };

  enum ErrorHandlingMode { NONE, THROW_EXCEPTION, BACKGROUND_ERROR };

  TclEvalCmd(const char* tcl_cmd, ErrorHandlingMode mode = NONE,
				 int flags = TCL_EVAL_GLOBAL) :
	 itsCmdObj(Tcl_NewStringObj(tcl_cmd, -1)),
	 itsErrorMode(mode),
	 itsFlags(flags) {}

  TclEvalCmd(Tcl_Obj* cmd_object, ErrorHandlingMode mode = NONE,
				 int flags = TCL_EVAL_GLOBAL) :
	 itsCmdObj(cmd_object),
	 itsErrorMode(mode),
	 itsFlags(flags) {}

  int invoke(Tcl_Interp* interp) { 
	 int tclresult = Tcl_EvalObjEx(interp, itsCmdObj, itsFlags);

	 if (NONE == itsErrorMode) { return tclresult; }

	 else {
		if (tclresult != TCL_OK) {
		  EvalError err(itsCmdObj);

		  if (THROW_EXCEPTION == itsErrorMode) {
			 throw err;
		  }
		  else if (BACKGROUND_ERROR == itsErrorMode) {
			 Tcl_AppendResult(interp, err.msg_cstr(), (char*) 0);
			 Tcl_BackgroundError(interp);
		  }
		}
	 }

	 return tclresult;
  }

private:
  TclObjPtr itsCmdObj;
  ErrorHandlingMode itsErrorMode;
  int itsFlags;
};

static const char vcid_tclevalcmd_h[] = "$Header$";
#endif // !TCLEVALCMD_H_DEFINED
