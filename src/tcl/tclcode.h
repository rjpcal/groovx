///////////////////////////////////////////////////////////////////////
//
// tclevalcmd.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Jun 17 10:38:13 1999
// written: Tue Dec 10 12:20:38 2002
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

#include "tcl/tclobjptr.h"
#include "tcl/tclsafeinterp.h"

#include "util/pointers.h"

namespace Util
{
  class ErrorHandler;
}

namespace Tcl
{
  class Code;
  class Interp;
}

/// Wraps a Tcl object containing executable code
/** Offers several error-handling strategies for the Tcl code. */
class Tcl::Code
{
public:
  class EvalError;

  Code();

  Code(const Tcl::ObjPtr& cmd, ErrorHandlingMode mode = IGNORE_ERRORS);

  Code(const Tcl::ObjPtr& cmd, Util::ErrorHandler* errHandler);

  /// Returns true on success, false on failure.
  bool invoke(const Tcl::Interp& interp);

private:
  Tcl::ObjPtr itsCodeObj;
  ErrorHandlingMode itsErrorMode;
  borrowed_ptr<Util::ErrorHandler> itsErrHandler;
};

static const char vcid_tclevalcmd_h[] = "$Header$";
#endif // !TCLCODE_H_DEFINED
