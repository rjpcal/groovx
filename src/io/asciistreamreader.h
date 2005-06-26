///////////////////////////////////////////////////////////////////////
//
// asciistreamreader.h
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Mon Jun  7 12:54:54 1999
// commit: $Id$
// $HeadURL$
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

#ifndef ASCIISTREAMREADER_H_DEFINED
#define ASCIISTREAMREADER_H_DEFINED

#include "rutz/sharedptr.h"

#include <iosfwd>

namespace IO
{
  class Reader;

  /// Make an AsciiStream reader that reads from \c std::ostream.
  /** \c AsciiStreamReader implements the \c IO::Reader interface, using
      nice human-readable ascii formatting. \c AsciiStreamReader restores
      objects from an input stream containing ascii data that has been
      generated by AsciiStreamWriter. With this data format, objects may
      read and write their attributes in any order. */
  rutz::shared_ptr<IO::Reader> makeAsciiStreamReader(std::istream& os);

  /// Make an AsciiStream reader that reads from the file named \a filename.
  rutz::shared_ptr<IO::Reader> makeAsciiStreamReader(const char* filename);
}

static const char vcid_asciistreamreader_h[] = "$Id$ $URL$";
#endif // !ASCIISTREAMREADER_H_DEFINED
