///////////////////////////////////////////////////////////////////////
//
// ioerror.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Sep 12 21:43:15 2001
// written: Wed Sep 12 21:45:54 2001
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
