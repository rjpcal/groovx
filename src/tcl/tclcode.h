///////////////////////////////////////////////////////////////////////
//
// tclevalcmd.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Jun 17 10:38:13 1999
// written: Tue Dec  7 23:45:54 1999
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

#ifndef STRING_DEFINED
#include <string>
#define STRING_DEFINED
#endif

#ifndef TCLOBJLOCK_H_DEFINED
#include "tclobjlock.h"
#endif

#ifndef TCLERROR_H_DEFINED
#include "tclerror.h"
#endif

namespace Tcl {
  class TclEvalCmd;
}

class Tcl::TclEvalCmd {
public:
  enum ErrorHandlingMode { NONE, THROW_EXCEPTION, BACKGROUND_ERROR };

  TclEvalCmd(const string& tcl_cmd, ErrorHandlingMode mode = NONE,
				 int flags = TCL_EVAL_GLOBAL) :
	 itsCmdObj(Tcl_NewStringObj(tcl_cmd.c_str(), -1)),
	 itsLock(itsCmdObj),
	 itsErrorMode(mode),
	 itsFlags(flags) {}

  TclEvalCmd(Tcl_Obj* cmd_object, ErrorHandlingMode mode = NONE,
				 int flags = TCL_EVAL_GLOBAL) :
	 itsCmdObj(cmd_object),
	 itsLock(itsCmdObj),
	 itsErrorMode(mode),
	 itsFlags(flags) {}

  int invoke(Tcl_Interp* interp) { 
	 int tclresult = Tcl_EvalObjEx(interp, itsCmdObj, itsFlags);

	 if (NONE == itsErrorMode) { return tclresult; }

	 else {
		if (tclresult != TCL_OK) {
		  string msg("error while evaluating ");
		  msg += Tcl_GetString(itsCmdObj);
		  
		  if (THROW_EXCEPTION == itsErrorMode) {
			 throw Tcl::TclError(msg);
		  }
		  else if (BACKGROUND_ERROR == itsErrorMode) {
			 Tcl_AppendResult(interp, msg.c_str(), (char*) 0);
			 Tcl_BackgroundError(interp);
		  }
		}
	 }

	 return tclresult;
  }

private:
  Tcl_Obj* itsCmdObj;
  Tcl::TclObjLock itsLock;
  ErrorHandlingMode itsErrorMode;
  int itsFlags;
};

static const char vcid_tclevalcmd_h[] = "$Header$";
#endif // !TCLEVALCMD_H_DEFINED
