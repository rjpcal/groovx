///////////////////////////////////////////////////////////////////////
//
// error.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Jun 22 14:59:48 1999
// written: Thu Sep 13 11:32:13 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef ERROR_CC_DEFINED
#define ERROR_CC_DEFINED

#include "util/error.h"

#include "util/strings.h"

#include <exception>

#include "util/trace.h"
#include "util/debug.h"


Util::Error::Error(const Util::Error& other) :
  itsInfo(other.itsInfo)
{
DOTRACE("Util::Error::Error(copy)");
  DebugEvalNL(itsInfo);
}

Util::Error::~Error()
{
DOTRACE("Util::Error::~Error");
}

static const char vcid_error_cc[] = "$Header$";
#endif // !ERROR_CC_DEFINED
