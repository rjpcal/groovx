///////////////////////////////////////////////////////////////////////
//
// irixsound.h
//
// Copyright (c) 1999-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Thu Oct 14 11:23:12 1999
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

#ifndef IRIXSOUND_H_DEFINED
#define IRIXSOUND_H_DEFINED

#include "io/ioerror.h"
#include "io/reader.h"
#include "io/writer.h"

#include "util/arrays.h"
#include "util/strings.h"

#include <dmedia/audio.h>
#include <dmedia/audiofile.h>
#include <unistd.h>

#include "util/trace.h"
#include "util/debug.h"

//  ###################################################################
//  ===================================================================
//
/// IrixAudioSound implements Sound using SGI's "dmedia" audio API.
//
//  ===================================================================

class IrixAudioSoundRep : public SoundRep
{
public:
  IrixAudioSoundRep(const char* filename = 0);
  virtual ~IrixAudioSoundRep() throw();

  virtual void play();

private:
  ALconfig itsAudioConfig;
  dynamic_block<unsigned char> itsSamples;
  AFframecount itsFrameCount;
};

IrixAudioSoundRep::IrixAudioSoundRep(const char* filename) :
  itsAudioConfig(alNewConfig()),
  itsSamples(),
  itsFrameCount(0)
{
DOTRACE("IrixAudioSoundRep::IrixAudioSoundRep");

  if (itsAudioConfig == 0)
    throw Util::Error("error creating an ALconfig while creating Sound", SRC_POS);

  SoundRep::checkFilename(filename);

  // Open filename as an audio file for reading. We pass a NULL
  // AFfilesetup to indicate that file setup parameters should be
  // taken from the file itself.
  AFfilehandle audiofile = afOpenFile(filename, "r", (AFfilesetup) 0);
  if (audiofile == AF_NULL_FILEHANDLE)
    {
      throw Util::Error(fstring("couldn't open sound file ", filename), SRC_POS);
    }

  // Read important parameters from the audio file, and use them to
  // set the corresponding parameters in itsAudioConfig.

  // Number of audio channels (i.e. mono == 1, stereo == 2)
  const int numChannels = afGetChannels(audiofile, AF_DEFAULT_TRACK);
  if (numChannels == -1)
    {
      throw Util::Error(fstring("error reading the number of channels "
                                "in sound file ", filename), SRC_POS);
    }
  alSetChannels(itsAudioConfig, numChannels);

  // Frame count
  itsFrameCount = afGetFrameCount(audiofile, AF_DEFAULT_TRACK);
  if (itsFrameCount < 0)
    {
      throw Util::Error(fstring("error reading the frame count "
                                "in sound file ", filename), SRC_POS);
    }

  // Sample format and width
  int sampleFormat = 0;
  int sampleWidth = 0;
  afGetSampleFormat(audiofile, AF_DEFAULT_TRACK,
                    &sampleFormat, &sampleWidth);
  alSetSampFmt(itsAudioConfig, sampleFormat);
  alSetWidth(itsAudioConfig, sampleWidth);

  // Sample size and rate
  const int bytesPerSample = (sampleWidth + 7)/8;

  const double samplingRate = afGetRate(audiofile, AF_DEFAULT_TRACK);

  ALpv pv;
  pv.param = AL_RATE;
  pv.value.ll = alDoubleToFixed(samplingRate);

  alSetParams(AL_DEFAULT_DEVICE, &pv, 1);

  dbgEval(3, numChannels);
  dbgEval(3, itsFrameCount);
  dbgEval(3, sampleWidth);
  dbgEval(3, bytesPerSample);
  dbgEvalNL(3, samplingRate);

  // Allocate space for the sound samples
  itsSamples.resize(itsFrameCount*numChannels*bytesPerSample);

  const int readResult = afReadFrames(audiofile, AF_DEFAULT_TRACK,
                                      static_cast<void*>(&itsSamples[0]),
                                      itsFrameCount);

  const int closeResult = afCloseFile(audiofile);

  if (readResult == -1)
    {
      throw Util::Error(fstring("error reading sound data "
                                "from file ", filename), SRC_POS);
    }

  if (closeResult == -1)
    {
      throw Util::Error(fstring("error closing sound file ", filename), SRC_POS);
    }
}

IrixAudioSoundRep::~IrixAudioSoundRep() throw()
{
DOTRACE("IrixAudioSoundRep::~IrixAudioSoundRep");
  if (itsAudioConfig != 0)
    {
      alFreeConfig(itsAudioConfig);
    }
}

void IrixAudioSoundRep::play()
{
DOTRACE("IrixAudioSoundRep::play");
  if (itsSamples.size() == 0) return;

  ALport audioPort = alOpenPort("Sound::play", "w", itsAudioConfig);
  dbgEvalNL(3, (void*) audioPort);
  if (audioPort == 0)
    {
      throw Util::Error("error opening an audio port during Sound::play", SRC_POS);
    }

  int writeResult =
    alWriteFrames(audioPort, static_cast<void*>(&itsSamples[0]), itsFrameCount);
  if (writeResult == -1)
    {
      throw Util::Error("error writing to the audio port during Sound::play", SRC_POS);
    }

  while (alGetFilled(audioPort) > 0)
    {
      usleep(5000);
    }

  int closeResult = alClosePort(audioPort);
  if (closeResult == -1)
    {
      throw Util::Error("error closing the audio port during Sound::play", SRC_POS);
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

Sound* Sound::newPlatformSoundRep(const char* soundfile)
{
DOTRACE("Sound::newPlatformSound");
  return new IrixAudioSoundRep(soundfile);
}

static const char vcid_irixsound_h[] = "$Header$";
#endif // !IRIXSOUND_H_DEFINED
