///////////////////////////////////////////////////////////////////////
//
// error.h
//
// Copyright (c) 1999-2003 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Jun 22 14:59:47 1999
// written: Mon Jan 13 11:08:26 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef ERROR_H_DEFINED
#define ERROR_H_DEFINED

#include "util/strings.h"

namespace Util
{
  class BackTrace;
  class Error;
}

/**
 *
 * \c Util::Error is a basic error class that carries a string
 * message describing the error.
 *
 **/

class Util::Error
{
public:
  /// Default construct with an empty message string.
  Error();

  /// Construct with an error message.
  Error(const fstring& msg);

  /// Copy constructor.
  Error(const Error& other);

  /// Virtual destructor.
  virtual ~Error();

  /// Get the error message.
  fstring& msg() { return itsInfo; }

  /// Get the error message.
  const fstring& msg() const { return itsInfo; }

  /// Get the error message as a C-style string.
  const char* msg_cstr() const { return itsInfo.c_str(); }

  /// Get the stack back trace associated with this exception.
  const BackTrace& backTrace() const { return *itsBackTrace; }

  /// Get the most recently back trace used in constructing a Util::Error.
  static const BackTrace& lastBackTrace();

private:
  Error& operator=(const Error& other);

  fstring itsInfo;
  const BackTrace* itsBackTrace;
};

static const char vcid_error_h[] = "$Header$";
#endif // !ERROR_H_DEFINED
