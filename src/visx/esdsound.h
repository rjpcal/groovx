///////////////////////////////////////////////////////////////////////
//
// esdsound.h
//
// Copyright (c) 2001-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Thu May 24 18:13:53 2001
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

/// EsdSound implementats the Sound interface using the ESD API.
class EsdSound : public Sound
{
public:
  /// Construct with reference to the named sound file.
  EsdSound(const char* filename = 0);

  /// Virtual destructor.
  virtual ~EsdSound();

  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

  /// Play the sound (in this case using the ESD daemon).
  virtual void play();

  /// Change to refer to a new sound file.
  virtual void setFile(const char* filename);

  /// Get the name of the associated sound file.
  virtual const char* getFile() const { return itsFilename.c_str(); }

  virtual fstring ioTypename() const { return "Sound"; }

  /// Swap contents with another EsdSound object.
  void swap(EsdSound& other)
  {
    itsFilename.swap(other.itsFilename);
  }

private:
  fstring itsFilename;
};

///////////////////////////////////////////////////////////////////////
//
// EsdSound member definitions
//
///////////////////////////////////////////////////////////////////////

EsdSound::EsdSound(const char* filename) :
  itsFilename("")
{
DOTRACE("EsdSound::EsdSound");
  setFile(filename);
}

EsdSound::~EsdSound()
{
DOTRACE("EsdSound::~EsdSound");
}

void EsdSound::readFrom(IO::Reader* reader)
{
DOTRACE("EsdSound::readFrom");

  reader->readValue("filename", itsFilename);

  dbgEval(3, itsFilename.length()); dbgEvalNL(3, itsFilename);

  if (!itsFilename.is_empty())
    setFile(itsFilename.c_str());
}

void EsdSound::writeTo(IO::Writer* writer) const
{
DOTRACE("EsdSound::writeTo");

  writer->writeValue("filename", itsFilename);
}

void EsdSound::play()
{
DOTRACE("EsdSound::play");

  if (!itsFilename.is_empty())
    {
      int res = esd_play_file("", itsFilename.c_str(), 0);
      if (res == 0)
        throw IO::FilenameError(itsFilename.c_str());
    }
}

void EsdSound::setFile(const char* filename)
{
DOTRACE("EsdSound::setFile");
  if (filename != 0 && filename[0] != '\0')
    {
      // We just use afOpenFile to ensure that the filename refers to
      // a readable+valid file
      AFfilehandle audiofile = afOpenFile(filename, "r", (AFfilesetup) 0);

      if (audiofile == AF_NULL_FILEHANDLE)
        {
          throw Util::Error(fstring("couldn't open sound file '",
                                    filename, "'"));
        }

      int closeResult = afCloseFile(audiofile);

      if (closeResult == -1)
        {
          throw Util::Error(fstring("error closing sound file '",
                                    filename, "'"));
        }

      itsFilename = filename;
    }
}


///////////////////////////////////////////////////////////////////////
//
// Sound static member definitions
//
///////////////////////////////////////////////////////////////////////

namespace
{
  int ESD = -1;
}

bool Sound::initSound()
{
DOTRACE("Sound::initSound");
  ESD = esd_audio_open();
  return haveSound();
}

bool Sound::haveSound()
{
DOTRACE("Sound::haveSound");
#ifndef ESD_WORKAROUND
  return (ESD > 0);
#else
  // for some strange reason, sounds seem to play ok under Mac OS X even if
  // the esd_audio_open() call fails
  return true;
#endif
}

void Sound::closeSound()
{
DOTRACE("Sound::closeSound");
  if (haveSound())
    esd_audio_close();
}

Sound* Sound::make()
{
DOTRACE("Sound::make");
  return new EsdSound();
}

Sound* Sound::newPlatformSound(const char* soundfile)
{
DOTRACE("Sound::newPlatformSound");
  return new EsdSound(soundfile);
}

static const char vcid_esdsound_h[] = "$Header$";
#endif // !ESDSOUND_H_DEFINED
