///////////////////////////////////////////////////////////////////////
//
// hpsound.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Oct 12 13:03:47 1999
// written: Mon Mar  6 19:29:03 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef HPSOUND_CC_DEFINED
#define HPSOUND_CC_DEFINED

#include "reader.h"
#include "writer.h"
#include "util/strings.h"

#include <Alib.h>
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
  const string_literal ioTag = "HpAudioSound";

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
  HpAudioSound(Audio* audio, const char* filename);
  virtual ~HpAudioSound();

  virtual void serialize(ostream& os, IOFlag flag) const;
  virtual void deserialize(istream& is, IOFlag flag);
  virtual int charCount() const;
  
  virtual void readFrom(Reader* reader);
  virtual void writeTo(Writer* writer) const;

  virtual void play();
  virtual void setFile(const char* filename);
  virtual const char* getFile() const { return itsFilename.c_str(); }

private:
  Audio* itsAudio;
  fixed_string itsFilename;
  SBucket* itsSBucket;
  SBPlayParams itsPlayParams;
};

///////////////////////////////////////////////////////////////////////
//
// HpAudioSound member definitions
//
///////////////////////////////////////////////////////////////////////

HpAudioSound::HpAudioSound(Audio* audio, const char* filename) :
  itsAudio(audio), itsSBucket(NULL)
{
DOTRACE("HpAudioSound::HpAudioSound");
  if ( !itsAudio ) { throw SoundError("invalid HP audio server connection"); }

  itsPlayParams.pause_first = 0; 
  itsPlayParams.start_offset.type = ATTSamples; 
  itsPlayParams.start_offset.u.samples = 0;
  itsPlayParams.loop_count = 0; 
  itsPlayParams.previous_transaction = 0; 
  itsPlayParams.gain_matrix = *ASimplePlayer(itsAudio);
  itsPlayParams.priority = APriorityNormal;
  itsPlayParams.play_volume = AUnityGain;
  itsPlayParams.duration.type = ATTFullLength;
  itsPlayParams.event_mask = 0;

  setFile(filename);
}

HpAudioSound::~HpAudioSound() {
DOTRACE("HpAudioSound::~HpAudioSound");
  ADestroySBucket( itsAudio, itsSBucket, NULL );
}

void HpAudioSound::serialize(ostream& os, IOFlag flag) const {
DOTRACE("HpAudioSound::serialize");

  char sep = ' ';
  if (flag & TYPENAME) { os << ioTag << sep; }

  os << itsFilename << endl;

  if (os.fail()) throw OutputError(ioTag.c_str());

  if (flag & BASES) { /* no bases to deal with */ }
}

void HpAudioSound::deserialize(istream& is, IOFlag flag) {
DOTRACE("HpAudioSound::deserialize");

  if (flag & TYPENAME) { IO::readTypename(is, ioTag.c_str()); }

  getline(is, itsFilename, '\n');
  
  if (is.fail()) throw InputError(ioTag.c_str());

  if (flag & BASES) { /* no bases to deal with */ }

  setFile(itsFilename.c_str());
}

void HpAudioSound::readFrom(Reader* reader) {
DOTRACE("HpAudioSound::readFrom");

  reader->readValue("filename", itsFilename);
  setFile(itsFilename.c_str());
}

void HpAudioSound::writeTo(Writer* writer) const {
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
  if ( !itsAudio ) { throw SoundError("invalid audio server connection"); }

  ATransID xid = APlaySBucket( itsAudio, itsSBucket, &itsPlayParams, NULL );
}
	
void HpAudioSound::setFile(const char* filename) {
DOTRACE("HpAudioSound::setFile");
  if ( !itsAudio ) { throw SoundError("invalid audio server connection"); }

  ifstream ifs(filename);
  if (ifs.fail()) {
	 throw SoundFilenameError(filename);
  }
  ifs.close();
  itsFilename = filename;

  if (itsSBucket) {
	 ADestroySBucket( itsAudio, itsSBucket, NULL);
	 itsSBucket = 0;
  }

  AFileFormat fileFormat = AFFUnknown;
  AudioAttrMask AttribsMask = 0;
  AudioAttributes Attribs;

  itsSBucket = ALoadAFile(itsAudio, const_cast<char *>(itsFilename.c_str()),
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
  return new HpAudioSound(theAudio, soundfile);
}

static const char vcid_hpsound_cc[] = "$Header$";
#endif // !HPSOUND_CC_DEFINED
