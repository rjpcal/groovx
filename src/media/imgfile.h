///////////////////////////////////////////////////////////////////////
//
// imgfile.h
//
// Copyright (c) 2002-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Thu Apr 25 09:06:46 2002
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

#ifndef IMGFILE_H_DEFINED
#define IMGFILE_H_DEFINED

namespace Gfx
{
  class BmapData;
}

namespace ImgFile
{
  /// Load \a data from \a filename, which must be in a supported format.
  void load(const char* filename, Gfx::BmapData& data);

  /// Save \a data to \a filename (file format is inferred from the filename).
  void save(const char* filename, const Gfx::BmapData& data);
}

static const char vcid_imgfile_h[] = "$Header$";
#endif // !IMGFILE_H_DEFINED