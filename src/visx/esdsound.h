///////////////////////////////////////////////////////////////////////
//
// esdsound.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu May 24 18:13:53 2001
// written: Wed Aug  8 15:29:29 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef ESDSOUND_H_DEFINED
#define ESDSOUND_H_DEFINED

#include "io/reader.h"
#include "io/writer.h"

#include "util/error.h"
#include "util/strings.h"

#include <esd.h>

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// EsdSound class definition --
//
// This is an implementation of the Sound interface using HP's Audio
// API.
//
///////////////////////////////////////////////////////////////////////

class EsdSound : public Sound {
public:
  EsdSound(const char* filename = 0);
  virtual ~EsdSound();

  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

  virtual void play();
  virtual void setFile(const char* filename);
  virtual const char* getFile() const { return itsFilename.c_str(); }

  virtual fixed_string ioTypename() const { return "Sound"; }

  void swap(EsdSound& other)
  {
    itsFilename.swap(other.itsFilename);
  }

private:
  fixed_string itsFilename;
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

  DebugEval(itsFilename.length()); DebugEvalNL(itsFilename);

  if (!itsFilename.empty())
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

  if (!itsFilename.empty())
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
          throw Util::Error("couldn't open sound file '", filename, "'");
        }

      int closeResult = afCloseFile(audiofile);

      if (closeResult == -1)
        {
          throw Util::Error("error closing sound file '", filename, "'");
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
  return (ESD > 0);
}

void Sound::closeSound()
{
DOTRACE("Sound::closeSound");
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
