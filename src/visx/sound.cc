/** @file visx/sound.cc simple playable sound objects */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Thu Jul  8 11:43:07 1999
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

#include "sound.h"

#include "io/reader.h"
#include "io/writer.h"

#include "nub/ref.h"

#include "rutz/error.h"

#if defined(HAVE_ALIB_H)
#  include "media/hpsound.h"
   typedef hp_audio_sound_rep sound_rep_t;

#elif defined(HAVE_DMEDIA_AUDIO_H)
#  include "media/irixsound.h"
   typedef media::irix_audio_sound_rep sound_rep_t;

#elif defined(HAVE_AUDIOTOOLBOX)
#  include "media/coreaudiosound.h"
   typedef media::coreaudio_sound_rep sound_rep_t;

#elif defined(HAVE_ESD_H)
#  include "media/esdsound.h"
   typedef media::esd_sound_rep sound_rep_t;

#else
#  include "media/dummysound.h"
   typedef media::dummy_sound_rep sound_rep_t;

#endif

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

namespace
{
  nub::soft_ref<Sound> theOkSound;
  nub::soft_ref<Sound> theErrSound;
}

void Sound::setOkSound(nub::ref<Sound> ok_sound)
{
  theOkSound = ok_sound;
}

void Sound::setErrSound(nub::ref<Sound> err_sound)
{
  theErrSound = err_sound;
}

nub::ref<Sound> Sound::getOkSound()
{
  return nub::ref<Sound>(theOkSound.get());
}

nub::ref<Sound> Sound::getErrSound()
{
  return nub::ref<Sound>(theErrSound.get());
}

Sound* Sound::make()
{
GVX_TRACE("Sound::make");
  return new Sound;
}

Sound* Sound::makeFrom(const char* filename)
{
GVX_TRACE("Sound::make");
  return new Sound(filename);
}

Sound::Sound(const char* filename) :
  itsFilename(""),
  itsRep()
{
GVX_TRACE("Sound::Sound");
  setFile(filename);
}

Sound::~Sound() noexcept
{
GVX_TRACE("Sound::~Sound");
}

void Sound::read_from(io::reader& reader)
{
GVX_TRACE("Sound::read_from");

  reader.read_value("filename", itsFilename);

  if (!itsFilename.is_empty())
    setFile(itsFilename.c_str());
}

void Sound::write_to(io::writer& writer) const
{
GVX_TRACE("Sound::write_to");

  writer.write_value("filename", itsFilename);
}

void Sound::play()
{
GVX_TRACE("Sound::play");

  forceLoad();

  if (itsRep.get() != nullptr)
    itsRep->play();
}

void Sound::forceLoad()
{
GVX_TRACE("Sound::forceLoad");
  // check if we have a valid filename without a corresponding valid rep
  if (itsRep.get() == nullptr && !itsFilename.is_empty())
    itsRep.reset(new sound_rep_t(itsFilename.c_str()));
}

void Sound::setFile(const char* filename)
{
GVX_TRACE("Sound::setFile");

  if (filename == nullptr)
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
GVX_TRACE("Sound::getFile");
  return itsFilename.c_str();
}
