///////////////////////////////////////////////////////////////////////
//
// irixsound.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Oct 14 11:23:12 1999
// written: Fri Sep 29 15:01:20 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IRIXSOUND_CC_DEFINED
#define IRIXSOUND_CC_DEFINED

#include "io/iolegacy.h"
#include "io/reader.h"
#include "io/writer.h"

#include "util/arrays.h"
#include "util/strings.h"

#include <dmedia/audio.h>
#include <dmedia/audiofile.h>
#include <fstream.h>				  // to check if files exist
#include <unistd.h>

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// IrixAudioSound class definition --
//
// This is pretty much a stub implementation of the Sound interface,
// which eventually will be filled in using some to-be-determined SGI
// sound API.
//
///////////////////////////////////////////////////////////////////////

class SoundFilenameError : public SoundError {
public:
  SoundFilenameError() : SoundError() {}
  SoundFilenameError(const char* filename) :
	 SoundError("bad or nonexistent file '")
	 {
		appendMsg(filename);
		appendMsg("'");
	 }
};

class IrixAudioSound : public Sound {
public:
  IrixAudioSound(const char* filename);
  virtual ~IrixAudioSound();

  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

  virtual void play();
  virtual void setFile(const char* filename);
  virtual const char* getFile() const { return itsFilename.c_str(); }

private:
  IrixAudioSound(const IrixAudioSound&);
  IrixAudioSound& operator=(const IrixAudioSound&);

  void legacySrlz(IO::LegacyWriter* writer) const;
  void legacyDesrlz(IO::LegacyReader* reader);

  fixed_string itsFilename;

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
  if (itsAudioConfig == 0) {
	 throw SoundError("error creating an ALconfig while creating Sound");
  }

  setFile(filename);
}

IrixAudioSound::~IrixAudioSound() {
DOTRACE("IrixAudioSound::~IrixAudioSound");
  if (itsAudioConfig != 0) {
	 alFreeConfig(itsAudioConfig);
  }
}

void IrixAudioSound::legacySrlz(IO::LegacyWriter* writer) const {
DOTRACE("IrixAudioSound::legacySrlz");
  IO::LegacyWriter* lwriter = dynamic_cast<IO::LegacyWriter*>(writer);
  if (lwriter != 0) {
	 lwriter->setStringMode(IO::GETLINE_NEWLINE);
	 writer->writeValue("filename", itsFilename);
  }
}

void IrixAudioSound::legacyDesrlz(IO::LegacyReader* reader) {
DOTRACE("IrixAudioSound::legacyDesrlz");
  IO::LegacyReader* lreader = dynamic_cast<IO::LegacyReader*>(reader); 
  if (lreader != 0) {
	 lreader->setStringMode(IO::GETLINE_NEWLINE);
	 reader->readValue("filename", itsFilename);

	 setFile(itsFilename.c_str());
  }
}

void IrixAudioSound::readFrom(IO::Reader* reader) {
DOTRACE("IrixAudioSound::readFrom");

  IO::LegacyReader* lreader = dynamic_cast<IO::LegacyReader*>(reader); 
  if (lreader != 0) {
	 legacyDesrlz(lreader);
	 return;
  }

  reader->readValue("filename", itsFilename);
  setFile(itsFilename.c_str());
}

void IrixAudioSound::writeTo(IO::Writer* writer) const {
DOTRACE("IrixAudioSound::writeTo");

  IO::LegacyWriter* lwriter = dynamic_cast<IO::LegacyWriter*>(writer);
  if (lwriter != 0) {
	 legacySrlz(lwriter);
	 return;
  }

  writer->writeValue("filename", itsFilename);
}

void IrixAudioSound::play() {
DOTRACE("IrixAudioSound::play");
  if (itsSamples.size() == 0) return;

  ALport audioPort = alOpenPort("Sound::play", "w", itsAudioConfig);
  DebugEvalNL((void*) audioPort);
  if (audioPort == 0) {
	 throw SoundError("error opening an audio port during Sound::play");
  }

  int writeResult =
	 alWriteFrames(audioPort, static_cast<void*>(&itsSamples[0]), itsFrameCount);
  if (writeResult == -1) {
	 throw SoundError("error writing to the audio port during Sound::play");
  }

  while (alGetFilled(audioPort) > 0) {
	 usleep(5000);
  }

  int closeResult = alClosePort(audioPort);
  if (closeResult == -1) {
	 throw SoundError("error closing the audio port during Sound::play");
  }
}

void IrixAudioSound::setFile(const char* filename) {
DOTRACE("IrixAudioSound::setFile");
  STD_IO::ifstream ifs(filename);
  if (ifs.fail()) {
	 throw SoundFilenameError(filename);
  }
  ifs.close();

  // Open itsFilename as an audio file for reading. We pass a NULL
  // AFfilesetup to indicate that file setup parameters should be
  // taken from the file itself.
  AFfilehandle audiofile = afOpenFile(filename, "r", (AFfilesetup) 0);
  if (audiofile == AF_NULL_FILEHANDLE) {
	 SoundError err("couldn't open sound file ");
	 err.appendMsg(itsFilename.c_str());
	 throw err;
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
  if (itsNumChannels == -1) {
	 SoundError err("error reading the number of channels in sound file ");
	 err.appendMsg(itsFilename.c_str());
	 throw err;
  }
  alSetChannels(itsAudioConfig, itsNumChannels);

  // Frame count
  itsFrameCount = afGetFrameCount(audiofile, AF_DEFAULT_TRACK);
  if (itsFrameCount < 0) {
	 SoundError err("error reading the frame count in sound file ");
	 err.appendMsg(itsFilename.c_str());
	 throw err;
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

  DebugEval(itsNumChannels);
  DebugEval(itsFrameCount);
  DebugEval(itsSampleWidth);
  DebugEval(itsBytesPerSample);
  DebugEval(itsSamplingRate);

  // Allocate space for the sound samples
  itsSamples.resize(itsFrameCount*itsNumChannels*itsBytesPerSample);

  int readResult = afReadFrames(audiofile, AF_DEFAULT_TRACK,
										  static_cast<void*>(&itsSamples[0]),
										  itsFrameCount);
  DebugEvalNL(readResult);

  int closeResult = afCloseFile(audiofile);

  // If the read failed, we dump any data stored in itsSamples
  if (readResult == -1) {
	 itsSamples.resize(0);
	 SoundError err("error reading sound data from file ");
	 err.appendMsg(itsFilename.c_str());
	 throw err;
  }

  // If the close failed, we keep the data that were read, but report
  // the error
  if (closeResult == -1) {
	 SoundError err("error closing sound file ");
	 err.appendMsg(itsFilename.c_str());
	 throw err;
  }
}

///////////////////////////////////////////////////////////////////////
//
// Sound static member definitions
//
///////////////////////////////////////////////////////////////////////

bool Sound::initSound() {
DOTRACE("Sound::initSound");
  return true;
}

bool Sound::haveSound() {
DOTRACE("Sound::haveSound");
  return true;
}

void Sound::closeSound() {
DOTRACE("Sound::closeSound");
}

Sound* Sound::newPlatformSound(const char* soundfile) {
DOTRACE("Sound::newPlatformSound");
  return new IrixAudioSound(soundfile);
}

static const char vcid_irixsound_cc[] = "$Header$";
#endif // !IRIXSOUND_CC_DEFINED
