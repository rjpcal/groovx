/** @file rutz/compressstream.h */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2007-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Wed Dec 14 23:54:45 2016
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

#ifndef GROOVX_RUTZ_COMPRESSSTREAM_H_UTC20161215075445_DEFINED
#define GROOVX_RUTZ_COMPRESSSTREAM_H_UTC20161215075445_DEFINED

#include <ios>
#include <iosfwd>
#include <memory>

namespace rutz
{
  class fstring;

  /** Opens a file for writing, with bzip2 compression if the filename
      ends with ".bz2", or gzip compression if the filename ends in
      ".gz". */
  std::unique_ptr<std::ostream> ocompressopen(const rutz::fstring& filename,
                                              std::ios::openmode flags =
                                              std::ios::openmode(0));

  /** Opens a file for reading, with bzip2 compression if the filename
      ends with ".bz2", or gzip compression if the filename ends in
      ".gz". */
  std::unique_ptr<std::istream> icompressopen(const rutz::fstring& filename,
                                              std::ios::openmode flags =
                                              std::ios::openmode(0));
}

#endif // !GROOVX_RUTZ_COMPRESSSTREAM_H_UTC20161215075445DEFINED
