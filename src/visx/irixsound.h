///////////////////////////////////////////////////////////////////////
//
// irixsound.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Oct 14 11:23:12 1999
// written: Fri Mar  3 23:20:56 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IRIXSOUND_CC_DEFINED
#define IRIXSOUND_CC_DEFINED

#include "reader.h"
#include "writer.h"

#include <dmedia/audio.h>
#include <dmedia/audiofile.h>
#include <fstream.h>				  // to check if files exist
#include <unistd.h>
#include <string>
#include <vector>

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

///////////////////////////////////////////////////////////////////////
//
// File scope definitions
//
///////////////////////////////////////////////////////////////////////

namespace {
  const string ioTag = "IrixAudioSound";
}

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
  SoundFilenameError(const string& filename) :
	 SoundError(string("bad or nonexistent file '") + filename + "'") {}
};

class IrixAudioSound : public Sound {
public:
  IrixAudioSound(const string& filename);
  virtual ~IrixAudioSound();

  virtual void serialize(ostream& os, IOFlag flag) const;
  virtual void deserialize(istream& is, IOFlag flag);
  virtual int charCount() const;
  
  virtual void readFrom(Reader* reader);
  virtual void writeTo(Writer* writer) const;

  virtual void play();
  virtual void setFile(const string& filename);
  virtual const string& getFile() const { return itsFilename; }

private:
  string itsFilename;

  ALconfig itsAudioConfig;
  int itsNumChannels;
  AFframecount itsFrameCount;
  int itsSampleFormat;
  int itsSampleWidth;
  int itsBytesPerSample;
  double itsSamplingRate;
  vector<unsigned char> itsSamples;
};

IrixAudioSound::IrixAudioSound(const string& filename) :
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

void IrixAudioSound::serialize(ostream& os, IOFlag flag) const {
DOTRACE("IrixAudioSound::serialize");

  char sep = ' ';
  if (flag & TYPENAME) { os << ioTag << sep; }

  os << itsFilename << endl;

  if (os.fail()) throw OutputError(ioTag);

  if (flag & BASES) { /* no bases to deal with */ }
}

void IrixAudioSound::deserialize(istream& is, IOFlag flag) {
DOTRACE("IrixAudioSound::deserialize");

  if (flag & TYPENAME) { IO::readTypename(is, ioTag); }

  getline(is, itsFilename, '\n');
  
  if (is.fail()) throw InputError(ioTag);

  if (flag & BASES) { /* no bases to deal with */ }

  setFile(itsFilename);
}

int IrixAudioSound::charCount() const {
DOTRACE("IrixAudioSound::charCount");
  return (ioTag.length() + 1
			 + itsFilename.length() + 1
			 +5); // fudge factor
}

void IrixAudioSound::readFrom(Reader* reader) {
DOTRACE("IrixAudioSound::readFrom");
  reader->readValue("filename", itsFilename);
  setFile(itsFilename);
}

void IrixAudioSound::writeTo(Writer* writer) const {
DOTRACE("IrixAudioSound::writeTo");
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

void IrixAudioSound::setFile(const string& filename) {
DOTRACE("IrixAudioSound::setFile");
  ifstream ifs(filename.c_str());
  if (ifs.fail()) {
	 throw SoundFilenameError(filename);
  }
  ifs.close();

  // Open itsFilename as an audio file for reading. We pass a NULL
  // AFfilesetup to indicate that file setup parameters should be
  // taken from the file itself.
  AFfilehandle audiofile = afOpenFile(filename.c_str(),
												  "r", (AFfilesetup) 0);
  if (audiofile == AF_NULL_FILEHANDLE) {
	 throw SoundError("couldn't open sound file " + itsFilename);
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
	 throw SoundError("error reading the number of channels in sound file " +
							itsFilename);
  }
  alSetChannels(itsAudioConfig, itsNumChannels);

  // Frame count
  itsFrameCount = afGetFrameCount(audiofile, AF_DEFAULT_TRACK);
  if (itsFrameCount < 0) {
	 throw SoundError("error reading the frame count in sound file " +
							itsFilename);
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
	 throw SoundError("error reading sound data from file " + itsFilename);
  }

  // If the close failed, we keep the data that were read, but report
  // the error
  if (closeResult == -1) {
	 throw SoundError("error closing sound file " + itsFilename);
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

Sound* Sound::newPlatformSound(const string& soundfile) {
DOTRACE("Sound::newPlatformSound");
  return new IrixAudioSound(soundfile);
}

static const char vcid_irixsound_cc[] = "$Header$";
#endif // !IRIXSOUND_CC_DEFINED
