///////////////////////////////////////////////////////////////////////
//
// esdsound.h
//
// Copyright (c) 2001-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Thu May 24 18:13:53 2001
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

#ifndef ESDSOUND_H_DEFINED
#define ESDSOUND_H_DEFINED

#include "media/soundrep.h"

#include "util/error.h"
#include "util/fstring.h"

#include <esd.h>

#include "util/trace.h"

namespace media
{
  /// esd_sound_rep implementats the Sound interface using the ESD API.
  class esd_sound_rep : public sound_rep
  {
  public:
    /// Construct with reference to the named sound file.
    esd_sound_rep(const char* filename = 0);

    /// Virtual destructor.
    virtual ~esd_sound_rep() throw();

    /// Play the sound (in this case using the ESD daemon).
    virtual void play();

  private:
    rutz::fstring m_filename;
  };

}

///////////////////////////////////////////////////////////////////////
//
// esd_sound_rep member definitions
//
///////////////////////////////////////////////////////////////////////

media::esd_sound_rep::esd_sound_rep(const char* filename) :
  m_filename("")
{
DOTRACE("media::esd_sound_rep::esd_sound_rep");

  sound_rep::check_filename(filename);

  // We just use afOpenFile to ensure that the filename refers to
  // a readable+valid file
  AFfilehandle audiofile = afOpenFile(filename, "r", (AFfilesetup) 0);

  if (audiofile == AF_NULL_FILEHANDLE)
    {
      throw rutz::error(rutz::fstring("couldn't open sound file '",
                                      filename, "'"), SRC_POS);
    }

  int close_result = afCloseFile(audiofile);

  if (close_result == -1)
    {
      throw rutz::error(rutz::fstring("error closing sound file '",
                                      filename, "'"), SRC_POS);
    }

  m_filename = filename;
}

media::esd_sound_rep::~esd_sound_rep() throw() {}

void media::esd_sound_rep::play()
{
  DOTRACE("media::esd_sound_rep::play");

  if (!m_filename.is_empty())
    {
      int res = esd_play_file("", m_filename.c_str(), 1);
      if (res == 0)
        throw rutz::error(rutz::fstring("error while attempting to "
                                        "play sound file:\n  '",
                                        m_filename.c_str(), "'"),
                          SRC_POS);
    }
}

static const char vcid_esdsound_h[] = "$Header$";
#endif // !ESDSOUND_H_DEFINED
