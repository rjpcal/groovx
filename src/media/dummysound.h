/** @file media/dummysound.h null class for when no sound API is available */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Tue Oct 12 13:03:47 1999
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

#ifndef GROOVX_MEDIA_DUMMYSOUND_H_UTC20050626084018_DEFINED
#define GROOVX_MEDIA_DUMMYSOUND_H_UTC20050626084018_DEFINED

#include "media/soundrep.h"

#include "rutz/trace.h"

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
GVX_TRACE("dummy_sound_rep::dummy_sound_rep");
  sound_rep::check_filename(filename);
}

void media::dummy_sound_rep::play()
{
GVX_TRACE("dummy_sound_rep::play");
}

static const char __attribute__((used)) vcid_groovx_media_dummysound_h_utc20050626084018[] = "$Id$ $HeadURL$";
#endif // !GROOVX_MEDIA_DUMMYSOUND_H_UTC20050626084018_DEFINED
