///////////////////////////////////////////////////////////////////////
//
// soundtcl.cc
// Rob Peters
// created: Tue Apr 13 14:09:59 1999
// written: Mon Jul 19 16:20:23 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef SOUNDTCL_CC_DEFINED
#define SOUNDTCL_CC_DEFINED

#include "soundtcl.h"

#include <tcl.h>
#include <map>
#include <Alib.h>
#include <string>
#include <fstream.h>				  // to check if files exist

#include "errmsg.h"
#include "soundlist.h"
#include "sound.h"
#include "listpkg.h"
#include "listitempkg.h"
#include "tcllink.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

namespace {
  const string ioTag = "HpAudioSound";
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
  SoundFilenameError(const string& filename) :
	 SoundError(string("bad or nonexistent file '") + filename + "'") {}
};

class HpAudioSound : public Sound {
public:
  HpAudioSound(Audio* audio, const string& filename);
  virtual ~HpAudioSound();

  virtual void serialize(ostream& os, IOFlag flag) const;
  virtual void deserialize(istream& is, IOFlag flag);
  virtual int charCount() const;
  
  virtual void play();
  virtual void setFile(const string& filename);
  virtual const string& getFile() const { return itsFilename; }

private:
  Audio* itsAudio;
  string itsFilename;
  SBucket* itsSBucket;
  SBPlayParams itsPlayParams;
};

///////////////////////////////////////////////////////////////////////
//
// HpAudioSound member definitions
//
///////////////////////////////////////////////////////////////////////

HpAudioSound::HpAudioSound(Audio* audio, const string& filename) :
  itsAudio(audio), itsSBucket(NULL)
{
DOTRACE("HpAudioSound::HpAudioSound");
  if ( !itsAudio ) { throw SoundError("invalid audio server connection"); }

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

  if (os.fail()) throw OutputError(ioTag);

  if (flag & BASES) { /* no bases to deal with */ }
}

void HpAudioSound::deserialize(istream& is, IOFlag flag) {
DOTRACE("HpAudioSound::deserialize");

  if (flag & TYPENAME) { IO::readTypename(is, ioTag); }

  getline(is, itsFilename, '\n');
  
  if (is.fail()) throw InputError(ioTag);

  if (flag & BASES) { /* no bases to deal with */ }

  setFile(itsFilename);
}

int HpAudioSound::charCount() const {
DOTRACE("HpAudioSound::charCount");
  return 0;
}
  
void HpAudioSound::play() {
DOTRACE("HpAudioSound::play");
  if ( !itsAudio ) { throw SoundError("invalid audio server connection"); }

  ATransID xid = APlaySBucket( itsAudio, itsSBucket, &itsPlayParams, NULL );
}
	
void HpAudioSound::setFile(const string& filename) {
DOTRACE("HpAudioSound::setFile");
  if ( !itsAudio ) { throw SoundError("invalid audio server connection"); }

  ifstream ifs(filename.c_str());
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

//---------------------------------------------------------------------
//
// SoundTcl namespace --
//
//---------------------------------------------------------------------

namespace SoundTcl {
  string ok_sound_file = "/cit/rjpeters/face/audio/saw50_500Hz_300ms.au";
  string err_sound_file = "/cit/rjpeters/face/audio/saw50_350Hz_300ms.au";

  Audio* theAudio;
  class SoundCmd;
  class HaveAudioCmd;
  class SoundPkg;
}

//---------------------------------------------------------------------
//
// SoundTcl::SoundCmd --
//
//---------------------------------------------------------------------

class SoundTcl::SoundCmd : public TclCmd {
public:
  SoundCmd(Tcl_Interp* interp, const char* cmd_name) :
	 TclCmd(interp, cmd_name, "filename", 2, 2) {}
protected:
  virtual void invoke() {
	 const char* filename = getCstringFromArg(1);

	 Sound* p = new HpAudioSound(theAudio, filename);
	 int id = SoundList::theSoundList().insert(p);
	 returnInt(id);
  }
};

//---------------------------------------------------------------------
//
// SoundTcl::HaveAudioCmd --
//
//---------------------------------------------------------------------

class SoundTcl::HaveAudioCmd : public TclCmd {
public:
  HaveAudioCmd(Tcl_Interp* interp, const char* cmd_name) :
	 TclCmd(interp, cmd_name, NULL, 1, 1) {}
protected:
  virtual void invoke() { returnBool( theAudio != 0 ); }
};

//---------------------------------------------------------------------
//
// SoundTcl::SoundPkg --
//
//---------------------------------------------------------------------

class SoundTcl::SoundPkg :
  public AbstractListItemPkg<HpAudioSound, SoundList> {
public:
  SoundPkg(Tcl_Interp* interp) :
	 AbstractListItemPkg<HpAudioSound, SoundList>(
			 interp, SoundList::theSoundList(), "Sound", "1.3")
  {
	 ASetErrorHandler(audioErrorHandler);

	 // Open an audio connection to the default audio server, then
	 // check to make sure connection succeeded. If the connection
	 // fails, 'audio' is set to NULL.
	 const char* ServerName = "";
	 long status = 0;
	 theAudio = AOpenAudio( const_cast<char *>(ServerName), &status );
	 if ( status != 0 ) {
		theAudio = NULL;
	 }

	 if ( theAudio ) {
		ASetCloseDownMode( theAudio, AKeepTransactions, NULL );

		try {
		  static int OK = 0;
		  theList().insertAt(OK, new HpAudioSound(theAudio, ok_sound_file));
		  linkConstVar("Sound::ok", OK);

		  static int ERR = 1;
		  theList().insertAt(ERR, new HpAudioSound(theAudio, err_sound_file));
		  linkConstVar("Sound::err", ERR);
		}
		catch (SoundError& err) {
		  DebugPrintNL("error creating sounds during startup");
		  Tcl_AppendStringsToObj(Tcl_GetObjResult(interp),
										 "SoundPkg: ", err.msg().c_str(), NULL);
		}

		addCommand( new SoundCmd(interp, "Sound::Sound") );
		addCommand( new HaveAudioCmd(interp, "Sound::haveAudio") );
		declareCAction("play", &HpAudioSound::play);
		declareCAttrib("file", &HpAudioSound::getFile, &HpAudioSound::setFile);
	 }
  }

  static long audioErrorHandler(Audio* audio, AErrorEvent *errEvent) {
	 static char buf[128];
	 AGetErrorText(audio, errEvent->error_code, buf, 127);
	 throw SoundError(string("HP Audio Error: ") + buf);
	 return 0;
  }
  
  ~SoundPkg() {
	 if ( theAudio ) {
		ACloseAudio( theAudio, NULL );
		theAudio = 0;
	 }
  }
};

//---------------------------------------------------------------------
//
// SoundListPkg --
//
//---------------------------------------------------------------------

namespace SoundListTcl {
  class SoundListPkg;
}

class SoundListTcl::SoundListPkg : public ListPkg<SoundList> {
public:
  SoundListPkg(Tcl_Interp* interp) :
	 ListPkg<SoundList>(interp, SoundList::theSoundList(),
							  "SoundList", "1.3") {}
};

//---------------------------------------------------------------------
//
// Sound_Init --
//
//---------------------------------------------------------------------

int Sound_Init(Tcl_Interp* interp) {
DOTRACE("Sound_Init");

  new SoundTcl::SoundPkg(interp);
  new SoundListTcl::SoundListPkg(interp);

  return TCL_OK;
}

static const char vcid_soundtcl_cc[] = "$Header$";
#endif // !SOUNDTCL_CC_DEFINED
