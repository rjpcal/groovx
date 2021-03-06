/** @file rutz/ioerror.h exception classes for input/output errors */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Wed Sep 12 21:42:09 2001
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [https://github.com/rjpcal/groovx]
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

#ifndef GROOVX_RUTZ_IOERROR_H_UTC20050626084019_DEFINED
#define GROOVX_RUTZ_IOERROR_H_UTC20050626084019_DEFINED

#include "rutz/error.h"

namespace rutz
{
  class filename_error;

  /// An exception class for invalid filenames or inaccessible files.
  class filename_error : public rutz::error
  {
  public:
    /// Default constructor.
    filename_error(const char* filename, const rutz::file_pos& pos);
  };

}

#endif // !GROOVX_RUTZ_IOERROR_H_UTC20050626084019_DEFINED
