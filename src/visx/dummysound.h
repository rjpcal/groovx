///////////////////////////////////////////////////////////////////////
//
// dummysound.h
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Tue Oct 12 13:03:47 1999
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

#ifndef DUMMYSOUND_H_DEFINED
#define DUMMYSOUND_H_DEFINED

#include "io/ioerror.h"
#include "io/reader.h"
#include "io/writer.h"

#include "util/strings.h"

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER;

/// DummySoundRep is a stub implementation of the SoundRep interface.
class DummySoundRep : public SoundRep
{
public:
  /// Construct from a sound file (but all we do is see if the file exists).
  DummySoundRep(const char* filename = 0);

  /// Play the sound (but this is a no-op for DummySound).
  virtual void play();
};

DummySoundRep::DummySoundRep(const char* filename)
{
DOTRACE("DummySoundRep::DummySoundRep");
  SoundRep::checkFilename(filename);
}

void DummySoundRep::play()
{
DOTRACE("DummySoundRep::play");
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
DOTRACE("Sound::newPlatformSoundRep");
  return new DummySoundRep(soundfile);
}

static const char vcid_dummysound_h[] = "$Header$";
#endif // !DUMMYSOUND_H_DEFINED
