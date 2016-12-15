/** @file rutz/compressstream.cc */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2007-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Wed Dec 14 23:57:16 2016
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

#include "rutz/compressstream.h"

#include "rutz/bzip2stream.h"
#include "rutz/error.h"
#include "rutz/fstring.h"
#include "rutz/gzstreambuf.h"
#include "rutz/sfmt.h"

#include <fstream>

std::unique_ptr<std::ostream>
rutz::ocompressopen(const rutz::fstring& filename,
                    std::ios::openmode flags)
{
  static fstring gz_ext(".gz");
  static fstring bz2_ext(".bz2");

  if (filename.ends_with(bz2_ext))
    {
      return rutz::obzip2open(filename, flags);
    }
  else if (filename.ends_with(gz_ext))
    {
      return rutz::ogzopen(filename, flags);
    }
  else
    {
      std::unique_ptr<std::ostream> result =
        std::make_unique<std::ofstream>(filename.c_str(), flags);
      if (result->fail())
        throw rutz::error(rutz::sfmt("couldn't open file '%s' "
                                     "for writing", filename.c_str()),
                          SRC_POS);

      return result;
    }
}

std::unique_ptr<std::istream>
rutz::icompressopen(const rutz::fstring& filename,
                    std::ios::openmode flags)
{
  static fstring bz2_ext(".bz2");

  if (filename.ends_with(bz2_ext))
    {
      return rutz::ibzip2open(filename, flags);
    }
  else
    {
      return rutz::igzopen(filename, flags);
    }
}
