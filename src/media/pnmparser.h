///////////////////////////////////////////////////////////////////////
//
// pbm.h
//
// Copyright (c) 1999-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Tue Jun 15 16:41:06 1999
// commit: $Id$
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

#ifndef PBM_H_DEFINED
#define PBM_H_DEFINED

#ifdef HAVE_IOSFWD
#  include <iosfwd>
#else
class istream;
class ostream;
#endif

namespace media
{
  class bmap_data;

  /// Load \a data in PBM format from the file \a filename.
  void load_pnm(const char* filename, media::bmap_data& data);

  /// Load \a data in PBM format from the \c std::ostream \a os.
  void load_pnm(STD_IO::istream& is, media::bmap_data& data);

  /// Save \a data in PBM format to the file \a filename.
  void save_pnm(const char* filename, const media::bmap_data& data);

  /// Save \a data in PBM format to the \c std::ostream \a os.
  void save_pnm(STD_IO::ostream& os, const media::bmap_data& data);
};

static const char vcid_pbm_h[] = "$Header$";
#endif // !PBM_H_DEFINED
