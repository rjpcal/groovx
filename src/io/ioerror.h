///////////////////////////////////////////////////////////////////////
//
// ioerror.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Sep 12 21:42:09 2001
// written: Thu Sep 13 10:57:24 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IOERROR_H_DEFINED
#define IOERROR_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(ERROR_H_DEFINED)
#include "util/error.h"
#endif

namespace IO
{
  class FilenameError;
}

/**
 * A subclass of Util::Error for invalid filenames or filenames
 * referring to inaccessible files.
 **/
class IO::FilenameError : public Util::Error
{
public:
  /// Default constructor.
  FilenameError(const char* filename);

  /// Virtual destructor
  virtual ~FilenameError();
};

static const char vcid_ioerror_h[] = "$Header$";
#endif // !IOERROR_H_DEFINED
