///////////////////////////////////////////////////////////////////////
//
// irixsound.h
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Thu Oct 14 11:23:12 1999
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

#ifndef IRIXSOUND_H_DEFINED
#define IRIXSOUND_H_DEFINED

#include "io/ioerror.h"
#include "io/reader.h"
#include "io/writer.h"

#include "util/arrays.h"
#include "util/strings.h"

#include <dmedia/audio.h>
#include <dmedia/audiofile.h>
#include <fstream.h>            // to check if files exist
#include <unistd.h>

#include "util/trace.h"
#include "util/debug.h"

//  ###################################################################
//  ===================================================================
//
/// IrixAudioSound implements Sound using SGI's "dmedia" audio API.
//
//  ===================================================================

class IrixAudioSound : public Sound
{
public:
  IrixAudioSound(const char* filename = 0);
  virtual ~IrixAudioSound();

  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

  virtual void play();
  virtual void setFile(const char* filename);
  virtual const char* getFile() const { return itsFilename.c_str(); }

  virtual fstring objTypename() const { return "Sound"; }

private:
  IrixAudioSound(const IrixAudioSound&);
  IrixAudioSound& operator=(const IrixAudioSound&);

  fstring itsFilename;

  ALconfig itsAudioConfig;
  int itsNumChannels;
  AFframecount itsFrameCount;
  int itsSampleFormat;
  int itsSampleWidth;
  int itsBytesPerSample;
  double itsSamplingRate;
  dynamic_block<unsigned char> itsSamples;
};

IrixAudioSound::IrixAudioSound(const char* filename) :
  itsFilename(""),
  itsAudioConfig(alNewConfig()),
  itsNumChannels(0),
  itsFrameCount(0),
  itsSampleFormat(0),
  itsSampleWidth(0),
  itsBytesPerSample(0),
  itsSamplingRate(0.0),
  itsSamples()
{
DOTRACE("IrixAudioSound::IrixAudioSound");
  if (itsAudioConfig == 0)
    throw Util::Error("error creating an ALconfig while creating Sound");

  if (filename)
    setFile(filename);
}

IrixAudioSound::~IrixAudioSound()
{
DOTRACE("IrixAudioSound::~IrixAudioSound");
  if (itsAudioConfig != 0)
    {
      alFreeConfig(itsAudioConfig);
    }
}

void IrixAudioSound::readFrom(IO::Reader* reader)
{
DOTRACE("IrixAudioSound::readFrom");

  reader->readValue("filename", itsFilename);
  setFile(itsFilename.c_str());
}

void IrixAudioSound::writeTo(IO::Writer* writer) const
{
DOTRACE("IrixAudioSound::writeTo");

  writer->writeValue("filename", itsFilename);
}

void IrixAudioSound::play()
{
DOTRACE("IrixAudioSound::play");
  if (itsSamples.size() == 0) return;

  ALport audioPort = alOpenPort("Sound::play", "w", itsAudioConfig);
  dbgEvalNL(3, (void*) audioPort);
  if (audioPort == 0)
    {
      throw Util::Error("error opening an audio port during Sound::play");
    }

  int writeResult =
    alWriteFrames(audioPort, static_cast<void*>(&itsSamples[0]), itsFrameCount);
  if (writeResult == -1)
    {
      throw Util::Error("error writing to the audio port during Sound::play");
    }

  while (alGetFilled(audioPort) > 0)
    {
      usleep(5000);
    }

  int closeResult = alClosePort(audioPort);
  if (closeResult == -1)
    {
      throw Util::Error("error closing the audio port during Sound::play");
    }
}

void IrixAudioSound::setFile(const char* filename)
{
DOTRACE("IrixAudioSound::setFile");
  STD_IO::ifstream ifs(filename);
  if (ifs.fail())
    {
      throw IO::FilenameError(filename);
    }
  ifs.close();

  // Open itsFilename as an audio file for reading. We pass a NULL
  // AFfilesetup to indicate that file setup parameters should be
  // taken from the file itself.
  AFfilehandle audiofile = afOpenFile(filename, "r", (AFfilesetup) 0);
  if (audiofile == AF_NULL_FILEHANDLE)
    {
      throw Util::Error(fstring("couldn't open sound file ", itsFilename));
    }

  // We don't actually set itsFilename to the new value until we are
  // sure that the file exists and is readable as a sound file.
  itsFilename = filename;

  // Having reset the filename, we are past the point of no return, so
  // we dump any old data that were in itsSamples.
  itsSamples.resize(0);

  // Read important parameters from the audio file, and use them to
  // set the corresponding parameters in itsAudioConfig.

  // Number of audio channels (i.e. mono == 1, stereo == 2)
  itsNumChannels = afGetChannels(audiofile, AF_DEFAULT_TRACK);
  if (itsNumChannels == -1)
    {
      throw Util::Error(fstring("error reading the number of channels "
                                "in sound file ", itsFilename));
    }
  alSetChannels(itsAudioConfig, itsNumChannels);

  // Frame count
  itsFrameCount = afGetFrameCount(audiofile, AF_DEFAULT_TRACK);
  if (itsFrameCount < 0)
    {
      throw Util::Error(fstring("error reading the frame count "
                                "in sound file ", itsFilename));
    }

  // Sample format and sample width
  afGetSampleFormat(audiofile, AF_DEFAULT_TRACK,
                    &itsSampleFormat, &itsSampleWidth);
  alSetSampFmt(itsAudioConfig, itsSampleFormat);
  alSetWidth(itsAudioConfig, itsSampleWidth);

  itsBytesPerSample = (itsSampleWidth + 7)/8;

  // Sampling rate
  itsSamplingRate = afGetRate(audiofile, AF_DEFAULT_TRACK);

  ALpv pv;
  pv.param = AL_RATE;
  pv.value.ll = alDoubleToFixed(itsSamplingRate);

  alSetParams(AL_DEFAULT_DEVICE, &pv, 1);

  dbgEval(3, itsNumChannels);
  dbgEval(3, itsFrameCount);
  dbgEval(3, itsSampleWidth);
  dbgEval(3, itsBytesPerSample);
  dbgEval(3, itsSamplingRate);

  // Allocate space for the sound samples
  itsSamples.resize(itsFrameCount*itsNumChannels*itsBytesPerSample);

  int readResult = afReadFrames(audiofile, AF_DEFAULT_TRACK,
                                static_cast<void*>(&itsSamples[0]),
                                itsFrameCount);
  dbgEvalNL(3, readResult);

  int closeResult = afCloseFile(audiofile);

  // If the read failed, we dump any data stored in itsSamples
  if (readResult == -1)
    {
      itsSamples.resize(0);
      throw Util::Error(fstring("error reading sound data "
                                "from file ", itsFilename));
    }

  // If the close failed, we keep the data that were read, but report
  // the error
  if (closeResult == -1)
    {
      throw Util::Error(fstring("error closing sound file ", itsFilename));
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
  return new IrixAudioSound();
}

Sound* Sound::newPlatformSound(const char* soundfile)
{
DOTRACE("Sound::newPlatformSound");
  return new IrixAudioSound(soundfile);
}

static const char vcid_irixsound_h[] = "$Header$";
#endif // !IRIXSOUND_H_DEFINED
