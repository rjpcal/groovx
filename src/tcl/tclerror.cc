///////////////////////////////////////////////////////////////////////
//
// tclerror.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sun Jun 20 15:10:13 1999
// written: Tue Dec 10 11:54:42 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLERROR_CC_DEFINED
#define TCLERROR_CC_DEFINED

#include "tcl/tclerror.h"

#include "util/trace.h"
#include "util/debug.h"


Tcl::TclError::~TclError() {}


void Tcl::BkdErrorHandler::raiseBackgroundError(const char* msg)
{
DOTRACE("Tcl::BkdErrorHandler::raiseBackgroundError");
  itsInterp.appendResult(msg);
  itsInterp.backgroundError();
}

Tcl::BkdErrorHandler::BkdErrorHandler(const Tcl::Interp& interp) :
  itsInterp(interp)
{
DOTRACE("Tcl::BkdErrorHandler::BkdErrorHandler");
}

Tcl::BkdErrorHandler::~BkdErrorHandler() {}

void Tcl::BkdErrorHandler::handleUnknownError()
{
  raiseBackgroundError("an error of unknown type occurred");
}

void Tcl::BkdErrorHandler::handleError(Util::Error& err)
{
  raiseBackgroundError(err.msg_cstr());
}

void Tcl::BkdErrorHandler::handleMsg(const char* msg)
{
  raiseBackgroundError(msg);
}

static const char vcid_tclerror_cc[] = "$Header$";
#endif // !TCLERROR_CC_DEFINED
