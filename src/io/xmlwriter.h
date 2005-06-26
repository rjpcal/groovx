///////////////////////////////////////////////////////////////////////
//
// xmlwriter.h
//
// Copyright (c) 2003-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Thu Jun 19 18:26:15 2003
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

#ifndef GROOVX_IO_XMLWRITER_H_UTC20050626084021_DEFINED
#define GROOVX_IO_XMLWRITER_H_UTC20050626084021_DEFINED

#include "rutz/sharedptr.h"

#include <iosfwd>

namespace IO
{
  class Writer;

  /// Make an XML writer that writes to \c std::ostream.
  rutz::shared_ptr<IO::Writer> makeXMLWriter(std::ostream& os);

  /// Make an XML writer that writes to the file named \a filename.
  rutz::shared_ptr<IO::Writer> makeXMLWriter(const char* filename);
}

static const char vcid_groovx_io_xmlwriter_h_utc20050626084021[] = "$Id$ $HeadURL$";
#endif // !GROOVX_IO_XMLWRITER_H_UTC20050626084021_DEFINED
