///////////////////////////////////////////////////////////////////////
//
// ioerror.h
//
// Copyright (c) 2001-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Wed Sep 12 21:42:09 2001
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

#ifndef IOERROR_H_DEFINED
#define IOERROR_H_DEFINED

#include "util/error.h"

namespace rutz
{
  class filename_error;

  /// An exception class for invalid filenames or inaccessible files.
  class filename_error : public rutz::error
  {
  public:
    /// Default constructor.
    filename_error(const char* filename, const rutz::file_pos& pos);

    /// Virtual destructor
    virtual ~filename_error() throw();
  };

}

static const char vcid_ioerror_h[] = "$Id$ $URL$";
#endif // !IOERROR_H_DEFINED
