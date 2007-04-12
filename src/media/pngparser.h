/** @file media/pngparser.h load/save png images */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Wed Apr 24 20:04:52 2002
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

#ifndef GROOVX_MEDIA_PNGPARSER_H_UTC20050626084018_DEFINED
#define GROOVX_MEDIA_PNGPARSER_H_UTC20050626084018_DEFINED

namespace media
{
  class bmap_data;

  /// Load \a data in PNG format from the file \a filename.
  void load_png(const char* filename, media::bmap_data& data);

  /// Write \a data in PNG format to the file \a filename.
  void save_png(const char* filename, const media::bmap_data& data);
}

static const char __attribute__((used)) vcid_groovx_media_pngparser_h_utc20050626084018[] = "$Id$ $HeadURL$";
#endif // !GROOVX_MEDIA_PNGPARSER_H_UTC20050626084018_DEFINED
