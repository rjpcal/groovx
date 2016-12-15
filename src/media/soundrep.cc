/** @file media/soundrep.cc abstract interface for playable sounds;
    subclasses implement the interface using platform-specific sound
    APIs */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Wed Oct 20 11:54:23 2004
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

#include "soundrep.h"

#include "rutz/ioerror.h"

#include <fstream>            // to check if files exist

#include "rutz/trace.h"

media::sound_rep::~sound_rep() noexcept {}

void media::sound_rep::check_filename(const char* filename)
{
GVX_TRACE("sound_rep::check_filename");

  if (filename == nullptr || filename[0] == '\0')
    throw rutz::error("invalid filename", SRC_POS);

  std::ifstream ifs(filename);

  if (ifs.fail())
    {
      throw rutz::filename_error(filename, SRC_POS);
    }
}
