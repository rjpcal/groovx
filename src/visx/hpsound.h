///////////////////////////////////////////////////////////////////////
//
// hpsound.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Oct 12 13:03:47 1999
// written: Sat Sep 23 15:32:25 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef HPSOUND_CC_DEFINED
#define HPSOUND_CC_DEFINED

#include "io/reader.h"
#include "io/writer.h"

#include "util/strings.h"

#include <Alib.h>
#include <fstream.h>				  // to check if files exist

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// File scope definitions
//
///////////////////////////////////////////////////////////////////////

namespace HPSOUND_CC_LOCAL {
  const string_literal ioTag("HpAudioSound");

  Audio* theAudio = 0;

  static long audioErrorHandler(Audio* audio, AErrorEvent *errEvent) {
	 static char buf[128];
	 AGetErrorText(audio, errEvent->error_code, buf, 127);

	 SoundError err("HP Audio Error: ");
	 err.appendMsg(buf);
	 throw err;

	 // we'll never get here, but we need to placate the compiler
	 return 0;
  }
}

using namespace HPSOUND_CC_LOCAL;

///////////////////////////////////////////////////////////////////////
//
// HpAudioSound class definition --
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

class HpAudioSound : public Sound {
public:
  HpAudioSound(const char* filename);
  virtual ~HpAudioSound();

  virtual void serialize(STD_IO::ostream& os, IO::IOFlag flag) const;
  virtual void deserialize(STD_IO::istream& is, IO::IOFlag flag);
  virtual int charCount() const;
  
  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

  virtual void play();
  virtual void setFile(const char* filename);
  virtual const char* getFile() const { return itsFilename.c_str(); }

private:
  fixed_string itsFilename;
  SBucket* itsSBucket;
  SBPlayParams itsPlayParams;
};

///////////////////////////////////////////////////////////////////////
//
// HpAudioSound member definitions
//
///////////////////////////////////////////////////////////////////////

HpAudioSound::HpAudioSound(const char* filename) :
  itsSBucket(NULL)
{
DOTRACE("HpAudioSound::HpAudioSound");
  if ( !theAudio ) { throw SoundError("invalid HP audio server connection"); }

  itsPlayParams.pause_first = 0; 
  itsPlayParams.start_offset.type = ATTSamples; 
  itsPlayParams.start_offset.u.samples = 0;
  itsPlayParams.loop_count = 0; 
  itsPlayParams.previous_transaction = 0; 
  itsPlayParams.gain_matrix = *ASimplePlayer(theAudio);
  itsPlayParams.priority = APriorityNormal;
  itsPlayParams.play_volume = AUnityGain;
  itsPlayParams.duration.type = ATTFullLength;
  itsPlayParams.event_mask = 0;

  setFile(filename);
}

HpAudioSound::~HpAudioSound() {
DOTRACE("HpAudioSound::~HpAudioSound");
  if ( theAudio != 0 ) {
	 ADestroySBucket( theAudio, itsSBucket, NULL );
  }
}

void HpAudioSound::serialize(STD_IO::ostream& os, IO::IOFlag flag) const {
DOTRACE("HpAudioSound::serialize");

  char sep = ' ';
  if (flag & IO::TYPENAME) { os << ioTag << sep; }

  os << itsFilename << endl;

  if (os.fail()) throw IO::OutputError(ioTag.c_str());

  if (flag & IO::BASES) { /* no bases to deal with */ }
}

void HpAudioSound::deserialize(STD_IO::istream& is, IO::IOFlag flag) {
DOTRACE("HpAudioSound::deserialize");

  if (flag & IO::TYPENAME) { IO::IoObject::readTypename(is, ioTag.c_str()); }

  getline(is, itsFilename, '\n');
  
  if (is.fail()) throw IO::InputError(ioTag.c_str());

  if (flag & IO::BASES) { /* no bases to deal with */ }

  setFile(itsFilename.c_str());
}

void HpAudioSound::readFrom(IO::Reader* reader) {
DOTRACE("HpAudioSound::readFrom");

  reader->readValue("filename", itsFilename);
  setFile(itsFilename.c_str());
}

void HpAudioSound::writeTo(IO::Writer* writer) const {
DOTRACE("HpAudioSound::writeTo");
  writer->writeValue("filename", itsFilename); 
}

int HpAudioSound::charCount() const {
DOTRACE("HpAudioSound::charCount");
  return (ioTag.length() + 1
			 + itsFilename.length() + 1
			 +5); // fudge factor
}
  
void HpAudioSound::play() {
DOTRACE("HpAudioSound::play");
  if ( !theAudio ) { throw SoundError("invalid audio server connection"); }

  ATransID xid = APlaySBucket( theAudio, itsSBucket, &itsPlayParams, NULL );
}
	
void HpAudioSound::setFile(const char* filename) {
DOTRACE("HpAudioSound::setFile");
  if ( !theAudio ) { throw SoundError("invalid audio server connection"); }

  STD_IO::ifstream ifs(filename);
  if (ifs.fail()) {
	 throw SoundFilenameError(filename);
  }
  ifs.close();
  itsFilename = filename;

  if (itsSBucket) {
	 ADestroySBucket( theAudio, itsSBucket, NULL);
	 itsSBucket = 0;
  }

  AFileFormat fileFormat = AFFUnknown;
  AudioAttrMask AttribsMask = 0;
  AudioAttributes Attribs;

  itsSBucket = ALoadAFile(theAudio, const_cast<char *>(itsFilename.c_str()),
								  fileFormat, AttribsMask,
								  &Attribs, NULL);
}


///////////////////////////////////////////////////////////////////////
//
// Sound static member definitions
//
///////////////////////////////////////////////////////////////////////

bool Sound::initSound() {
DOTRACE("Sound::initSound");
  if (theAudio != 0) return true;

  ASetErrorHandler(audioErrorHandler);
  
  bool retVal = false;

  // Open an audio connection to the default audio server, then
  // check to make sure connection succeeded. If the connection
  // fails, 'audio' is set to NULL.
  const char* ServerName = "";
  long status = 0;
  theAudio = AOpenAudio( const_cast<char *>(ServerName), &status );
  if ( status != 0 ) {
	 theAudio = NULL;
	 retVal = false;
  }
  else {
	 ASetCloseDownMode( theAudio, AKeepTransactions, NULL );
	 retVal = true;
  }

  return retVal;
}

bool Sound::haveSound() {
DOTRACE("Sound::haveSound");
  return (theAudio != 0);
}

void Sound::closeSound() {
DOTRACE("Sound::closeSound");
  if ( theAudio ) {
	 ACloseAudio( theAudio, NULL );
	 theAudio = 0;
  }
}

Sound* Sound::newPlatformSound(const char* soundfile) {
DOTRACE("Sound::newPlatformSound");
  return new HpAudioSound(soundfile);
}

static const char vcid_hpsound_cc[] = "$Header$";
#endif // !HPSOUND_CC_DEFINED
