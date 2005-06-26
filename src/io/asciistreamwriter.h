///////////////////////////////////////////////////////////////////////
//
// asciistreamwriter.h
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Mon Jun  7 13:05:56 1999
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

#ifndef ASCIISTREAMWRITER_H_DEFINED
#define ASCIISTREAMWRITER_H_DEFINED

#include "util/sharedptr.h"

#include <iosfwd>

namespace IO
{
  class Writer;

  /// Make an AsciiStream writer that writes to \c std::ostream.
  /** \c AsciiStreamWriter implements the \c IO::Writer interface, using
      nice human-readable ascii formatting. \c AsciiStreamWriter writes
      objects to an output stream in a ascii format that is readable by \c
      AsciiStreamReader. With this data format, objects may read and write
      their attributes in any order. */
  rutz::shared_ptr<IO::Writer> makeAsciiStreamWriter(std::ostream& os);

  /// Make an AsciiStream writer that writes to the file named \a filename.
  rutz::shared_ptr<IO::Writer> makeAsciiStreamWriter(const char* filename);
}

static const char vcid_asciistreamwriter_h[] = "$Id$ $URL$";
#endif // !ASCIISTREAMWRITER_H_DEFINED
