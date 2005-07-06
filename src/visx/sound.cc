/** @file visx/sound.cc simple playable sound objects */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Thu Jul  8 11:43:07 1999
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

#ifndef GROOVX_VISX_SOUND_CC_UTC20050626084015_DEFINED
#define GROOVX_VISX_SOUND_CC_UTC20050626084015_DEFINED

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

#elif defined(HAVE_QUICKTIME_MOVIES_H)
#  include "media/quicktimesound.h"
   typedef media::quicktime_sound_rep sound_rep_t;

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

Sound::~Sound() throw()
{
GVX_TRACE("Sound::~Sound");
}

void Sound::readFrom(IO::Reader& reader)
{
GVX_TRACE("Sound::readFrom");

  reader.readValue("filename", itsFilename);

  if (!itsFilename.is_empty())
    setFile(itsFilename.c_str());
}

void Sound::writeTo(IO::Writer& writer) const
{
GVX_TRACE("Sound::writeTo");

  writer.writeValue("filename", itsFilename);
}

void Sound::play()
{
GVX_TRACE("Sound::play");

  forceLoad();

  if (itsRep.get() != 0)
    itsRep->play();
}

void Sound::forceLoad()
{
GVX_TRACE("Sound::forceLoad");
  // check if we have a valid filename without a corresponding valid rep
  if (itsRep.get() == 0 && !itsFilename.is_empty())
    itsRep.reset(new sound_rep_t(itsFilename.c_str()));
}

void Sound::setFile(const char* filename)
{
GVX_TRACE("Sound::setFile");

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
GVX_TRACE("Sound::getFile");
  return itsFilename.c_str();
}

static const char vcid_groovx_visx_sound_cc_utc20050626084015[] = "$Id$ $HeadURL$";
#endif // !GROOVX_VISX_SOUND_CC_UTC20050626084015_DEFINED
