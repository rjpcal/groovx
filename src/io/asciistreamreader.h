/** @file io/asciistreamreader.h read io::serializable objects in the ASW
    format */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Mon Jun  7 12:54:54 1999
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
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

#ifndef GROOVX_IO_ASCIISTREAMREADER_H_UTC20050626084021_DEFINED
#define GROOVX_IO_ASCIISTREAMREADER_H_UTC20050626084021_DEFINED

#include "rutz/shared_ptr.h"

#include <iosfwd>

namespace io
{
  class reader;

  /// Make an ASW reader that reads from the given std::ostream.
  /** The returned io::reader will restore objects from an input
      stream containing ASW-formatted data, which is a human-readable
      text format. With this data format, objects may read and write
      their attributes in any order. */
  rutz::shared_ptr<io::reader> make_asw_reader(std::istream& os);

  /// Make an ASW reader that reads from the named file.
  rutz::shared_ptr<io::reader> make_asw_reader(const char* filename);
}

#endif // !GROOVX_IO_ASCIISTREAMREADER_H_UTC20050626084021_DEFINED
