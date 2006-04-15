/** @file media/jpegparser.h load jpeg images (no save capability) */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Mon Nov 11 15:15:46 2002
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

#ifndef GROOVX_MEDIA_JPEGPARSER_H_UTC20050626084018_DEFINED
#define GROOVX_MEDIA_JPEGPARSER_H_UTC20050626084018_DEFINED

namespace media
{
  class bmap_data;

  /// Load \a data in JPEG format from the file \a filename.
  void load_jpeg(const char* filename, media::bmap_data& data);
}

static const char __attribute__((used)) vcid_groovx_media_jpegparser_h_utc20050626084018[] = "$Id$ $HeadURL$";
#endif // !GROOVX_MEDIA_JPEGPARSER_H_UTC20050626084018_DEFINED
