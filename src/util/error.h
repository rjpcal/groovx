///////////////////////////////////////////////////////////////////////
//
// error.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Jun 22 14:59:47 1999
// written: Sun Aug 26 08:35:10 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef ERROR_H_DEFINED
#define ERROR_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(STRINGS_H_DEFINED)
#include "util/strings.h"
#endif

namespace Util
{
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
  Error() : itsInfo() {}

  /// Construct with an error message.
  Error(const fstring& msg) : itsInfo(msg) {}

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

private:
  Error& operator=(const Error& other);

  fstring itsInfo;
};

static const char vcid_error_h[] = "$Header$";
#endif // !ERROR_H_DEFINED
