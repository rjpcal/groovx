///////////////////////////////////////////////////////////////////////
//
// error.h
//
// Copyright (c) 1999-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Tue Jun 22 14:59:47 1999
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef ERROR_H_DEFINED
#define ERROR_H_DEFINED

#include "util/fileposition.h"
#include "util/strings.h"

#include <exception>

namespace Util
{
  class BackTrace;
  class Error;
}

//  ####################################################################
/// \c Util::Error is a basic exception class.
/** It carries a string message describing the error as well as
    information about the location in the source code in which the
    exception was generated. */

class Util::Error : public std::exception
{
public:
  /// Default construct with an empty message string.
  Error(const FilePosition& pos);

  /// Construct with an error message.
  Error(const fstring& msg, const FilePosition& pos);

  /// Copy constructor.
  Error(const Error& other) throw();

  /// Virtual destructor.
  virtual ~Error() throw();

  /// Get the error message.
  fstring& msg() throw() { return itsInfo; }

  /// Get the error message.
  const fstring& msg() const throw() { return itsInfo; }

  /// Get the error message as a C-style string.
  virtual const char* what() const throw();

  /// Get the source file position where the error was generated.
  const FilePosition& srcPos() const throw() { return itsPos; }

  /// Get the stack back trace associated with this exception.
  const BackTrace& backTrace() const throw() { return *itsBackTrace; }

  /// Get the most recently back trace used in constructing a Util::Error.
  static const BackTrace& lastBackTrace();

private:
  Error& operator=(const Error& other);

  fstring itsInfo;
  FilePosition itsPos;
  const BackTrace* itsBackTrace;
};

static const char vcid_error_h[] = "$Header$";
#endif // !ERROR_H_DEFINED
