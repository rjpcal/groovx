///////////////////////////////////////////////////////////////////////
//
// tclevalcmd.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Jun 17 10:38:13 1999
// written: Wed Jul 11 18:35:28 2001
// $Id$
//
// Tcl::Code serves as a wrapper for a Tcl command string that is to
// be TclEval'd. It creates an object from the string, locks the
// object to ensure that it is not garbage collected, and provides a
// function to invoke the command in a Tcl_Interp. The command is
// byte-compiled by Tcl upon its first invocation.
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLCODE_H_DEFINED
#define TCLCODE_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TCLOBJPTR_H_DEFINED)
#include "tcl/tclobjptr.h"
#endif

struct Tcl_Obj;
struct Tcl_Interp;

namespace Util
{
  class ErrorHandler;
}

namespace Tcl
{
  class Code;
  class SafeInterp;
}

class Tcl::Code {
public:
  class EvalError;

  enum ErrorHandlingMode
  {
	 NONE,
	 THROW_EXCEPTION,
	 BACKGROUND_ERROR
  };

  Code();

  Code(Tcl::ObjPtr cmd, ErrorHandlingMode mode = NONE);

  Code(Tcl::ObjPtr cmd, Util::ErrorHandler* errHandler);

  /// Returns true on success, false on failure.
  bool invoke(Tcl_Interp* interp);

  /// Returns true on success, false on failure.
  bool invoke(const Tcl::SafeInterp& interp);

private:
  Tcl::ObjPtr itsCodeObj;
  ErrorHandlingMode itsErrorMode;
  int itsFlags;
  Util::ErrorHandler* itsErrHandler;
};

static const char vcid_tclevalcmd_h[] = "$Header$";
#endif // !TCLCODE_H_DEFINED
