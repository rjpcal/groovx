///////////////////////////////////////////////////////////////////////
//
// sound.cc
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Thu Jul  8 11:43:07 1999
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef SOUND_CC_DEFINED
#define SOUND_CC_DEFINED

#include "visx/sound.h"

#include "io/ioerror.h"
#include "io/reader.h"
#include "io/writer.h"

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

#include "util/trace.h"
#include "util/debug.h"

namespace
{
  SoftRef<Sound> OK_SOUND;
  SoftRef<Sound> ERR_SOUND;
}

SoundRep::~SoundRep() {}

Sound::~Sound() {}

void Sound::setOkSound(Ref<Sound> ok_sound)
{
  OK_SOUND = ok_sound;
}

void Sound::setErrSound(Ref<Sound> err_sound)
{
  ERR_SOUND = err_sound;
}

Ref<Sound> Sound::getOkSound()
{
  return Ref<Sound>(OK_SOUND.get());
}

Ref<Sound> Sound::getErrSound()
{
  return Ref<Sound>(ERR_SOUND.get());
}

StdSound::StdSound(const char* filename) :
  itsFilename(""),
  itsRep(0)
{
DOTRACE("StdSound::StdSound");
  setFile(filename);
}

StdSound::~StdSound()
{
DOTRACE("StdSound::~StdSound");
}

void StdSound::readFrom(IO::Reader* reader)
{
DOTRACE("StdSound::readFrom");

  reader->readValue("filename", itsFilename);

  if (!itsFilename.is_empty())
    setFile(itsFilename.c_str());
}

void StdSound::writeTo(IO::Writer* writer) const
{
DOTRACE("StdSound::writeTo");

  writer->writeValue("filename", itsFilename);
}

void StdSound::play()
{
DOTRACE("StdSound::play");
  if (itsRep.get() != 0)
    itsRep->play();
}

void StdSound::setFile(const char* filename)
{
DOTRACE("StdSound::setFile");
  if (filename != 0 && filename[0] != '\0')
    {
      itsRep.reset(newPlatformSoundRep(filename));
      itsFilename = filename;
    }
}

const char* StdSound::getFile() const
{
DOTRACE("StdSound::getFile");
  return itsFilename.c_str();
}

static const char vcid_sound_cc[] = "$Header$";
#endif // !SOUND_CC_DEFINED
