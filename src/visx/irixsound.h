///////////////////////////////////////////////////////////////////////
//
// irixsound.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Oct 14 11:23:12 1999
// written: Thu Oct 14 11:49:11 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IRIXSOUND_CC_DEFINED
#define IRIXSOUND_CC_DEFINED

#include <fstream.h>				  // to check if files exist

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

  bool haveAudio = false;
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
  IrixAudioSound(const string& filename) 
	 { setFile(filename); }
  virtual ~IrixAudioSound() {}

  virtual void serialize(ostream& os, IOFlag flag) const;
  virtual void deserialize(istream& is, IOFlag flag);
  virtual int charCount() const;
  
  virtual void play() { /* do nothing */ }
  virtual void setFile(const string& filename);
  virtual const string& getFile() const { return itsFilename; }

private:
  string itsFilename;
};

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

void IrixAudioSound::setFile(const string& filename) {
DOTRACE("IrixAudioSound::setFile");
  ifstream ifs(filename.c_str());
  if (ifs.fail()) {
	 throw SoundFilenameError(filename);
  }
  ifs.close();
}

///////////////////////////////////////////////////////////////////////
//
// Sound static member definitions
//
///////////////////////////////////////////////////////////////////////

bool Sound::initSound() {
DOTRACE("Sound::initSound");
  return false;
}

bool Sound::haveSound() {
DOTRACE("Sound::haveSound");
  return haveAudio;
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
