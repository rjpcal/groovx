///////////////////////////////////////////////////////////////////////
//
// soundrep.cc
//
// Copyright (c) 2004-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Wed Oct 20 11:54:23 2004
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

#ifndef SOUNDREP_CC_DEFINED
#define SOUNDREP_CC_DEFINED

#include "soundrep.h"

#include "util/ioerror.h"

#ifdef HAVE_FSTREAM
#include <fstream>            // to check if files exist
#else
#include <fstream.h>
#endif

#include "util/trace.h"

media::sound_rep::~sound_rep() throw() {}

void media::sound_rep::check_filename(const char* filename)
{
DOTRACE("sound_rep::check_filename");

  if (filename == 0 || filename[0] == '\0')
    throw rutz::error("invalid filename", SRC_POS);

  STD_IO::ifstream ifs(filename);

  if (ifs.fail())
    {
      throw rutz::filename_error(filename, SRC_POS);
    }
}

static const char vcid_soundrep_cc[] = "$Header$";
#endif // !SOUNDREP_CC_DEFINED