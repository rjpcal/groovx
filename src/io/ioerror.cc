///////////////////////////////////////////////////////////////////////
//
// ioerror.cc
//
// Copyright (c) 2001-2003 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Sep 12 21:43:15 2001
// written: Mon Jan 13 11:04:47 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IOERROR_CC_DEFINED
#define IOERROR_CC_DEFINED

#include "io/ioerror.h"

#include "util/trace.h"

IO::FilenameError::FilenameError(const char* str) :
  Util::Error(fstring("couldn't use file '", str, "'"))
{
DOTRACE("IO::FilenameError::FilenameError");
}

IO::FilenameError::~FilenameError() {}

static const char vcid_ioerror_cc[] = "$Header$";
#endif // !IOERROR_CC_DEFINED
