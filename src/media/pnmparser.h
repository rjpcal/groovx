/** @file media/pnmparser.h load/save pnm images (i.e., pbm, pgm, ppm) */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Tue Jun 15 16:41:06 1999
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

#ifndef GROOVX_MEDIA_PNMPARSER_H_UTC20050626084018_DEFINED
#define GROOVX_MEDIA_PNMPARSER_H_UTC20050626084018_DEFINED

#include <iosfwd>

namespace media
{
  class bmap_data;

  /// Load \a data in PBM format from the file \a filename.
  void load_pnm(const char* filename, media::bmap_data& data);

  /// Load \a data in PBM format from the \c std::ostream \a os.
  void load_pnm(std::istream& is, media::bmap_data& data);

  /// Save \a data in PBM format to the file \a filename.
  void save_pnm(const char* filename, const media::bmap_data& data);

  /// Save \a data in PBM format to the \c std::ostream \a os.
  void save_pnm(std::ostream& os, const media::bmap_data& data);
};

static const char __attribute__((used)) vcid_groovx_media_pnmparser_h_utc20050626084018[] = "$Id$ $HeadURL$";
#endif // !GROOVX_MEDIA_PNMPARSER_H_UTC20050626084018_DEFINED
