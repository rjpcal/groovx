///////////////////////////////////////////////////////////////////////
//
// error.h
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Tue Jun 22 14:59:47 1999
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
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
  Error(const Error& other) throw();

  /// Virtual destructor.
  virtual ~Error() throw();

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
