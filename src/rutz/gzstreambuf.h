/** @file rutz/gzstreambuf.h handle gzip-encoding through a c++
    iostreams interface */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Wed Jun 20 09:12:51 2001
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

#ifndef GROOVX_RUTZ_GZSTREAMBUF_H_UTC20050626084020_DEFINED
#define GROOVX_RUTZ_GZSTREAMBUF_H_UTC20050626084020_DEFINED

#include <istream>
#include <memory>
#include <ostream>
#include <streambuf>
#include <zlib.h>

namespace rutz
{
  class fstring;

  /** Opens a file for writing. An exception will be thrown if the
      specified file cannot be opened. The output file will be
      gz-compressed if the filename ends with ".gz". */
  std::unique_ptr<std::ostream> ogzopen(const rutz::fstring& filename,
                                        std::ios::openmode flags =
                                        std::ios::openmode(0));

  /** Opens a file for reading. An exception will be thrown if the
      specified file cannot be opened. If the file is gz-compressed,
      this will be automagically detected regardless of the filename
      extension. */
  std::unique_ptr<std::istream> igzopen(const rutz::fstring& filename,
                                        std::ios::openmode flags =
                                        std::ios::openmode(0));
}

#endif // !GROOVX_RUTZ_GZSTREAMBUF_H_UTC20050626084020_DEFINED
