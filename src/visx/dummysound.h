///////////////////////////////////////////////////////////////////////
//
// dummysound.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Oct 12 13:03:47 1999
// written: Thu May 10 12:04:40 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef DUMMYSOUND_H_DEFINED
#define DUMMYSOUND_H_DEFINED

#include "io/reader.h"
#include "io/writer.h"

#include "util/strings.h"

#include <fstream.h>				  // to check if files exist

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// DummySound class definition --
//
// This is an implementation of the Sound interface using HP's Audio
// API.
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

class DummySound : public Sound {
public:
  DummySound(const char* filename = 0);
  virtual ~DummySound();

  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

  virtual void play();
  virtual void setFile(const char* filename);
  virtual const char* getFile() const { return itsFilename.c_str(); }

  virtual fixed_string ioTypename() const { return "Sound"; }

  void swap(DummySound& other)
  {
	 itsFilename.swap(other.itsFilename);
  }

private:
  fixed_string itsFilename;
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
  if (filename != 0 && filename[0] != '\0')
	 {
		STD_IO::ifstream ifs(filename);
		if (ifs.fail()) {
		  throw SoundFilenameError(filename);
		}
		ifs.close();

		itsFilename = filename;
	 }
}

DummySound::~DummySound() {
DOTRACE("DummySound::~DummySound");
}

void DummySound::readFrom(IO::Reader* reader) {
DOTRACE("DummySound::readFrom");

  reader->readValue("filename", itsFilename);

  DebugEval(itsFilename.length()); DebugEvalNL(itsFilename);

  if (!itsFilename.empty())
	 setFile(itsFilename.c_str());
}

void DummySound::writeTo(IO::Writer* writer) const {
DOTRACE("DummySound::writeTo");

  writer->writeValue("filename", itsFilename); 
}

void DummySound::play() {
DOTRACE("DummySound::play");
}
	
void DummySound::setFile(const char* filename) {
DOTRACE("DummySound::setFile");

  DummySound new_sound(filename); 
  this->swap(new_sound);
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

Sound* Sound::make() {
DOTRACE("Sound::make");
  return new DummySound();
}

Sound* Sound::newPlatformSound(const char* soundfile) {
DOTRACE("Sound::newPlatformSound");
  return new DummySound(soundfile);
}

static const char vcid_dummysound_h[] = "$Header$";
#endif // !DUMMYSOUND_H_DEFINED
