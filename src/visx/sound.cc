///////////////////////////////////////////////////////////////////////
//
// sound.cc
//
// Copyright (c) 1999-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Thu Jul  8 11:43:07 1999
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

#ifndef SOUND_CC_DEFINED
#define SOUND_CC_DEFINED

#include "visx/sound.h"

#include "io/ioerror.h"
#include "io/reader.h"
#include "io/writer.h"

#include "util/error.h"
#include "util/ref.h"

#if defined(HAVE_ALIB_H)
#  include "visx/hpsound.h"
#elif defined(HAVE_DMEDIA_AUDIO_H)
#  include "visx/irixsound.h"
#elif defined(HAVE_QUICKTIME_MOVIES_H)
#  include "visx/quicktimesound.h"
#elif defined(HAVE_ESD_H)
#  include "visx/esdsound.h"
#else
#  include "visx/dummysound.h"
#endif

#ifdef HAVE_FSTREAM
#include <fstream>            // to check if files exist
#else
#include <fstream.h>
#endif

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER

namespace
{
  Util::SoftRef<Sound> theOkSound;
  Util::SoftRef<Sound> theErrSound;
}

SoundRep::~SoundRep() throw() {}

void SoundRep::checkFilename(const char* filename)
{
DOTRACE("SoundRep::checkFilename");

  if (filename == 0 || filename[0] == '\0')
    throw Util::Error("invalid filename");

  STD_IO::ifstream ifs(filename);

  if (ifs.fail())
    {
      throw IO::FilenameError(filename);
    }
}

void Sound::setOkSound(Util::Ref<Sound> ok_sound)
{
  theOkSound = ok_sound;
}

void Sound::setErrSound(Util::Ref<Sound> err_sound)
{
  theErrSound = err_sound;
}

Util::Ref<Sound> Sound::getOkSound()
{
  return Util::Ref<Sound>(theOkSound.get());
}

Util::Ref<Sound> Sound::getErrSound()
{
  return Util::Ref<Sound>(theErrSound.get());
}

Sound* Sound::make()
{
DOTRACE("Sound::make");
  return new Sound;
}

Sound* Sound::makeFrom(const char* filename)
{
DOTRACE("Sound::make");
  return new Sound(filename);
}

Sound::Sound(const char* filename) :
  itsFilename(""),
  itsRep()
{
DOTRACE("Sound::Sound");
  setFile(filename);
}

Sound::~Sound() throw()
{
DOTRACE("Sound::~Sound");
}

void Sound::readFrom(IO::Reader& reader)
{
DOTRACE("Sound::readFrom");

  reader.readValue("filename", itsFilename);

  if (!itsFilename.is_empty())
    setFile(itsFilename.c_str());
}

void Sound::writeTo(IO::Writer& writer) const
{
DOTRACE("Sound::writeTo");

  writer.writeValue("filename", itsFilename);
}

void Sound::play()
{
DOTRACE("Sound::play");

  forceLoad();

  if (itsRep.get() != 0)
    itsRep->play();
}

void Sound::forceLoad()
{
DOTRACE("Sound::forceLoad");
  // check if we have a valid filename without a corresponding valid rep
  if (itsRep.get() == 0 && !itsFilename.is_empty())
    itsRep.reset(newPlatformSoundRep(itsFilename.c_str()));
}

void Sound::setFile(const char* filename)
{
DOTRACE("Sound::setFile");

  if (filename == 0)
    filename = "";

  if (itsFilename != filename)
    {
      // itsRep is only lazily updated, so just mark it as invalid
      // here... we won't have to update it until someone actually tries to
      // play() the sound
      itsRep.reset();
      itsFilename = filename;
    }
}

const char* Sound::getFile() const
{
DOTRACE("Sound::getFile");
  return itsFilename.c_str();
}

static const char vcid_sound_cc[] = "$Header$";
#endif // !SOUND_CC_DEFINED
