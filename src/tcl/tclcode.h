///////////////////////////////////////////////////////////////////////
//
// tclevalcmd.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Jun 17 10:38:13 1999
// written: Tue Jun 29 11:49:56 1999
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

class TclEvalCmd {
public:
  TclEvalCmd(const string& tcl_cmd, int flags = TCL_EVAL_GLOBAL) :
	 itsCmdObj(Tcl_NewStringObj(tcl_cmd.c_str(), -1)),
	 itsLock(itsCmdObj),
	 itsFlags(flags) {}

  int invoke(Tcl_Interp* interp) { 
	 return Tcl_EvalObjEx(interp, itsCmdObj, itsFlags);
  }

private:
  Tcl_Obj* itsCmdObj;
  TclObjLock itsLock;
  int itsFlags;
};

static const char vcid_tclevalcmd_h[] = "$Header$";
#endif // !TCLEVALCMD_H_DEFINED
