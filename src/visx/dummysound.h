///////////////////////////////////////////////////////////////////////
//
// intelsound.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Oct 12 13:03:47 1999
// written: Thu May 10 12:04:40 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef HPSOUND_H_DEFINED
#define HPSOUND_H_DEFINED

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
// IntelSound class definition --
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

class IntelSound : public Sound {
public:
  IntelSound(const char* filename = 0);
  virtual ~IntelSound();

  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

  virtual void play();
  virtual void setFile(const char* filename);
  virtual const char* getFile() const { return itsFilename.c_str(); }

  virtual fixed_string ioTypename() const { return "Sound"; }

  void swap(IntelSound& other)
  {
	 itsFilename.swap(other.itsFilename);
  }

private:
  fixed_string itsFilename;
};

///////////////////////////////////////////////////////////////////////
//
// IntelSound member definitions
//
///////////////////////////////////////////////////////////////////////

IntelSound::IntelSound(const char* filename) :
  itsFilename("")
{
DOTRACE("IntelSound::IntelSound");
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

IntelSound::~IntelSound() {
DOTRACE("IntelSound::~IntelSound");
}

void IntelSound::readFrom(IO::Reader* reader) {
DOTRACE("IntelSound::readFrom");

  reader->readValue("filename", itsFilename);

  DebugEval(itsFilename.length()); DebugEvalNL(itsFilename);

  if (!itsFilename.empty())
	 setFile(itsFilename.c_str());
}

void IntelSound::writeTo(IO::Writer* writer) const {
DOTRACE("IntelSound::writeTo");

  writer->writeValue("filename", itsFilename); 
}

void IntelSound::play() {
DOTRACE("IntelSound::play");
}
	
void IntelSound::setFile(const char* filename) {
DOTRACE("IntelSound::setFile");

  IntelSound new_sound(filename); 
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
  return new IntelSound();
}

Sound* Sound::newPlatformSound(const char* soundfile) {
DOTRACE("Sound::newPlatformSound");
  return new IntelSound(soundfile);
}

static const char vcid_intelsound_h[] = "$Header$";
#endif // !HPSOUND_H_DEFINED
