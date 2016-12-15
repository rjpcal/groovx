/** @file media/pnmparser.h load/save pnm images (i.e., pbm, pgm, ppm) */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Tue Jun 15 16:41:06 1999
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

#ifndef GROOVX_MEDIA_PNMPARSER_H_UTC20050626084018_DEFINED
#define GROOVX_MEDIA_PNMPARSER_H_UTC20050626084018_DEFINED

#include <iosfwd>

namespace media
{
  class bmap_data;

  /// Load \a data in PBM format from the file \a filename.
  bmap_data load_pnm(const char* filename);

  /// Load \a data in PBM format from the \c std::ostream \a os.
  bmap_data load_pnm(std::istream& is);

  /// Save \a data in PBM format to the file \a filename.
  void save_pnm(const char* filename, const media::bmap_data& data);

  /// Save \a data in PBM format to the \c std::ostream \a os.
  void save_pnm(std::ostream& os, const media::bmap_data& data);
};

#endif // !GROOVX_MEDIA_PNMPARSER_H_UTC20050626084018_DEFINED
