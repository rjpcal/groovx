///////////////////////////////////////////////////////////////////////
//
// error.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Jun 22 14:59:48 1999
// written: Wed Jul 31 17:15:02 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef ERROR_CC_DEFINED
#define ERROR_CC_DEFINED

#include "util/error.h"

#include "util/strings.h"

#include <cstdlib>
#include <exception>

#include "util/trace.h"
#include "util/debug.h"

Util::Error::Error() :
  itsInfo(),
  itsBackTrace(new BackTrace(Util::BackTrace::current()))
{
DOTRACE("Util::Error::Error()");
}

Util::Error::Error(const fstring& msg) :
  itsInfo(msg),
  itsBackTrace(new BackTrace(Util::BackTrace::current()))
{
DOTRACE("Util::Error::Error(fstring)");
}

Util::Error::Error(const Util::Error& other) :
  itsInfo(other.itsInfo),
  itsBackTrace(new BackTrace(*other.itsBackTrace))
{
DOTRACE("Util::Error::Error(copy)");
  DebugEvalNL(itsInfo);
}

Util::Error::~Error()
{
DOTRACE("Util::Error::~Error");
  delete itsBackTrace;
}

static const char vcid_error_cc[] = "$Header$";
#endif // !ERROR_CC_DEFINED
