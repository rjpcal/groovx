/** @file io/xmlwriter.h io::writer implementation for writing XML
    files in the GVX format */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2003-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Thu Jun 19 18:26:15 2003
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

#ifndef GROOVX_IO_XMLWRITER_H_UTC20050626084021_DEFINED
#define GROOVX_IO_XMLWRITER_H_UTC20050626084021_DEFINED

#include <iosfwd>
#include <memory>

namespace io
{
  class writer;

  /// Make an XML writer that writes to \c std::ostream.
  std::unique_ptr<io::writer> make_xml_writer(std::ostream& os);

  /// Make an XML writer that writes to the file named \a filename.
  std::unique_ptr<io::writer> make_xml_writer(const char* filename);
}

#endif // !GROOVX_IO_XMLWRITER_H_UTC20050626084021_DEFINED
