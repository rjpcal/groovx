///////////////////////////////////////////////////////////////////////
//
// tclcode.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jul 16 13:29:16 2001
// written: Tue Dec 10 12:19:59 2002
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

Tcl::Code::Code() :
  itsCodeObj(),
  itsErrorMode(IGNORE_ERRORS),
  itsErrHandler()
{}

Tcl::Code::Code(const Tcl::ObjPtr& cmd, ErrorHandlingMode mode) :
  itsCodeObj(cmd),
  itsErrorMode(mode),
  itsErrHandler(0)
{}

Tcl::Code::Code(const Tcl::ObjPtr& cmd, Util::ErrorHandler* handler) :
  itsCodeObj(cmd),
  itsErrorMode(IGNORE_ERRORS),
  itsErrHandler(handler)
{}

bool Tcl::Code::invoke(const Tcl::Interp& interp)
{
DOTRACE("Tcl::Code::invoke");

  if (itsErrHandler != 0)
    return const_cast<Tcl::Interp&>(interp).invoke(itsCodeObj, itsErrHandler);
  else
    return const_cast<Tcl::Interp&>(interp).invoke(itsCodeObj, itsErrorMode);
}

static const char vcid_tclcode_cc[] = "$Header$";
#endif // !TCLCODE_CC_DEFINED
