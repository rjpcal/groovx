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

#include "io/ioerror.h"
#include "io/reader.h"
#include "io/writer.h"

#include "util/error.h"
#include "util/strings.h"

#include <esd.h>

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER

/// EsdSound implementats the Sound interface using the ESD API.
class EsdSoundRep : public SoundRep
{
public:
  /// Construct with reference to the named sound file.
  EsdSoundRep(const char* filename = 0);

  /// Virtual destructor.
  virtual ~EsdSoundRep() throw();

  /// Play the sound (in this case using the ESD daemon).
  virtual void play();

private:
  fstring itsFilename;
};

///////////////////////////////////////////////////////////////////////
//
// EsdSound member definitions
//
///////////////////////////////////////////////////////////////////////

EsdSoundRep::EsdSoundRep(const char* filename) :
  itsFilename("")
{
DOTRACE("EsdSoundRep::EsdSoundRep");

  SoundRep::checkFilename(filename);

  // We just use afOpenFile to ensure that the filename refers to
  // a readable+valid file
  AFfilehandle audiofile = afOpenFile(filename, "r", (AFfilesetup) 0);

  if (audiofile == AF_NULL_FILEHANDLE)
    {
      throw rutz::error(fstring("couldn't open sound file '",
                                filename, "'"), SRC_POS);
    }

  int closeResult = afCloseFile(audiofile);

  if (closeResult == -1)
    {
      throw rutz::error(fstring("error closing sound file '",
                                filename, "'"), SRC_POS);
    }

  itsFilename = filename;
}

EsdSoundRep::~EsdSoundRep() throw() {}

void EsdSoundRep::play()
{
DOTRACE("EsdSoundRep::play");

  if (!itsFilename.is_empty())
    {
      int res = esd_play_file("", itsFilename.c_str(), 1);
      if (res == 0)
        throw rutz::error(fstring("error while attempting to play "
                                  "sound file:\n  '",
                                  itsFilename.c_str(), "'"), SRC_POS);
    }
}


///////////////////////////////////////////////////////////////////////
//
// Sound static member definitions
//
///////////////////////////////////////////////////////////////////////

bool Sound::initSound()
{
DOTRACE("Sound::initSound");
  return true;
}

bool Sound::haveSound()
{
DOTRACE("Sound::haveSound");
  return true;
}

void Sound::closeSound()
{
DOTRACE("Sound::closeSound");
}

SoundRep* Sound::newPlatformSoundRep(const char* soundfile)
{
DOTRACE("SoundRep::newPlatformSoundRep");
  return new EsdSoundRep(soundfile);
}

static const char vcid_esdsound_h[] = "$Header$";
#endif // !ESDSOUND_H_DEFINED
