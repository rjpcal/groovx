///////////////////////////////////////////////////////////////////////
//
// dummysound.h
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Tue Oct 12 13:03:47 1999
// written: Wed Mar 19 18:00:57 2003
// $Id$
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

#include <fstream.h>            // to check if files exist

#include "util/trace.h"
#include "util/debug.h"

/// DummySound is a stub implementation of the Sound interface.
class DummySound : public Sound
{
public:
  DummySound(const char* filename = 0);
  virtual ~DummySound();

  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

  virtual void play();
  virtual void setFile(const char* filename);
  virtual const char* getFile() const { return itsFilename.c_str(); }

  virtual fstring ioTypename() const { return "Sound"; }

  void swap(DummySound& other)
  {
    itsFilename.swap(other.itsFilename);
  }

private:
  fstring itsFilename;
};

///////////////////////////////////////////////////////////////////////
//
// DummySound member definitions
//
///////////////////////////////////////////////////////////////////////

DummySound::DummySound(const char* filename) :
  itsFilename("")
{
DOTRACE("DummySound::DummySound");
  setFile(filename);
}

DummySound::~DummySound()
{
DOTRACE("DummySound::~DummySound");
}

void DummySound::readFrom(IO::Reader* reader)
{
DOTRACE("DummySound::readFrom");

  reader->readValue("filename", itsFilename);

  dbgEval(3, itsFilename.length()); dbgEvalNL(3, itsFilename);

  if (!itsFilename.is_empty())
    setFile(itsFilename.c_str());
}

void DummySound::writeTo(IO::Writer* writer) const
{
DOTRACE("DummySound::writeTo");

  writer->writeValue("filename", itsFilename);
}

void DummySound::play()
{
DOTRACE("DummySound::play");
}

void DummySound::setFile(const char* filename)
{
DOTRACE("DummySound::setFile");
  if (filename != 0 && filename[0] != '\0')
    {
      STD_IO::ifstream ifs(filename);
      if (ifs.fail())
        {
          throw IO::FilenameError(filename);
        }
      ifs.close();

      itsFilename = filename;
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

Sound* Sound::make()
{
DOTRACE("Sound::make");
  return new DummySound();
}

Sound* Sound::newPlatformSound(const char* soundfile)
{
DOTRACE("Sound::newPlatformSound");
  return new DummySound(soundfile);
}

static const char vcid_dummysound_h[] = "$Header$";
#endif // !DUMMYSOUND_H_DEFINED
