///////////////////////////////////////////////////////////////////////
//
// tclerror.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sun Jun 20 15:10:13 1999
// written: Thu May 10 12:04:42 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLERROR_CC_DEFINED
#define TCLERROR_CC_DEFINED

#include "tcl/tclerror.h"

#include <tcl.h>

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

namespace Tcl {

TclError::TclError() :
  ::ErrorWithMsg()
{
DOTRACE("TclError::TclError");
}

TclError::TclError(const char* msg) : 
  ::ErrorWithMsg(msg)
{
DOTRACE("TclError::TclError");
}

TclError::~TclError() {}

}


void Tcl::BkdErrorHandler::raiseBackgroundError(const char* msg) {
DOTRACE("Tcl::BkdErrorHandler::raiseBackgroundError");
  Tcl_AppendResult(itsInterp, msg, (char*) 0);
  Tcl_BackgroundError(itsInterp);
}

Tcl::BkdErrorHandler::BkdErrorHandler(Tcl_Interp* interp) :
  itsInterp(interp)
{
DOTRACE("Tcl::BkdErrorHandler::BkdErrorHandler");
  Invariant(itsInterp != 0);
}

Tcl::BkdErrorHandler::~BkdErrorHandler() {}

void Tcl::BkdErrorHandler::handleUnknownError() {
  raiseBackgroundError("an error of unknown type occurred");
}

void Tcl::BkdErrorHandler::handleError(Error&) {
  raiseBackgroundError("an Error of unknown type occurred");
}

void Tcl::BkdErrorHandler::handleErrorWithMsg(ErrorWithMsg& err) {
  raiseBackgroundError(err.msg_cstr());
}

void Tcl::BkdErrorHandler::handleMsg(const char* msg) {
  raiseBackgroundError(msg);
}

static const char vcid_tclerror_cc[] = "$Header$";
#endif // !TCLERROR_CC_DEFINED
