/** @file io/asciistreamwriter.h write io::serializable objects in the ASW
    format */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Mon Jun  7 13:05:56 1999
// commit: $Id$
// $HeadURL$
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

#ifndef GROOVX_IO_ASCIISTREAMWRITER_H_UTC20050626084021_DEFINED
#define GROOVX_IO_ASCIISTREAMWRITER_H_UTC20050626084021_DEFINED

#include "rutz/shared_ptr.h"

#include <iosfwd>

namespace io
{
  class writer;

  /// Make an ASW writer that writes to the given std::ostream.
  /** The returned io::writer serializes objects to an output stream
      in the human-readable ASW plaintext format. With this data
      format, objects may read and write their attributes in any
      order. */
  rutz::shared_ptr<io::writer> make_asw_writer(std::ostream& os);

  /// Make an ASW writer that writes to the named file.
  rutz::shared_ptr<io::writer> make_asw_writer(const char* filename);
}

static const char __attribute__((used)) vcid_groovx_io_asciistreamwriter_h_utc20050626084021[] = "$Id$ $HeadURL$";
#endif // !GROOVX_IO_ASCIISTREAMWRITER_H_UTC20050626084021_DEFINED
