///////////////////////////////////////////////////////////////////////
//
// dummysound.h
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Tue Oct 12 13:03:47 1999
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

#ifndef DUMMYSOUND_H_DEFINED
#define DUMMYSOUND_H_DEFINED

#include "media/soundrep.h"

#include "util/trace.h"

namespace media
{
  /// dummy_sound_rep is a stub implementation of the sound_rep interface.
  class dummy_sound_rep : public sound_rep
  {
  public:
    /// Construct from a sound file (but all we do is see if the file exists).
    dummy_sound_rep(const char* filename = 0);

    /// Play the sound (but this is a no-op for DummySound).
    virtual void play();
  };
}

media::dummy_sound_rep::dummy_sound_rep(const char* filename)
{
DOTRACE("dummy_sound_rep::dummy_sound_rep");
  sound_rep::check_filename(filename);
}

void media::dummy_sound_rep::play()
{
DOTRACE("dummy_sound_rep::play");
}

static const char vcid_dummysound_h[] = "$Id$ $URL$";
#endif // !DUMMYSOUND_H_DEFINED
