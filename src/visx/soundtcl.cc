///////////////////////////////////////////////////////////////////////
// soundtcl.cc
// Rob Peters
// created: Tue Apr 13 14:09:59 1999
// written: Fri Jul  9 19:12:54 1999
// $Id$
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

///////////////////////////////////////////////////////////////////////
// HpAudioSound class declaration
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

  virtual void serialize(ostream& os, IOFlag flag) const {}
  virtual void deserialize(istream& is, IOFlag flag) {}
  virtual int charCount() const { return 0; }
  
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
// HpAudioSound member definitions
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
  }

  AFileFormat fileFormat = AFFUnknown;
  AudioAttrMask AttribsMask = 0;
  AudioAttributes Attribs;

  itsSBucket = ALoadAFile(itsAudio, const_cast<char *>(itsFilename.c_str()),
								  fileFormat, AttribsMask,
								  &Attribs, NULL);
}

///////////////////////////////////////////////////////////////////////
// Accessories of Sound Tcl package
///////////////////////////////////////////////////////////////////////

namespace {
  map< string, HpAudioSound* > theSoundMap;

  string ok_sound_file = "/cit/rjpeters/face/audio/saw50_500Hz_300ms.au";
  string err_sound_file = "/cit/rjpeters/face/audio/saw50_350Hz_300ms.au";

  Audio* theAudio;

  // Error messages
  const char* const bad_sound_msg = "sound does not exist";
  const char* const no_audio_msg = "audio is not enabled";
}

///////////////////////////////////////////////////////////////////////
// Sound Tcl package definitions
///////////////////////////////////////////////////////////////////////

int SoundTcl::setCmd(ClientData, Tcl_Interp* interp,
							int objc, Tcl_Obj* const objv[]) {
DOTRACE("SoundTcl::setCmd");
  if ( !theAudio ) {
	 err_message(interp, objv, no_audio_msg);
	 return TCL_OK;
  }

  if (objc < 2 || objc > 3) {
	 Tcl_WrongNumArgs(interp, 1, objv, "sound_name ?new_filename?");
	 return TCL_ERROR;
  }
  
  string snd_name = Tcl_GetString(objv[1]);

  // Retrieve current filename
  if (objc == 2) {
	 if (!theSoundMap[snd_name]) {
		err_message(interp, objv, bad_sound_msg);
		return TCL_ERROR;
	 }
	 Tcl_SetObjResult(interp,
		    Tcl_NewStringObj(theSoundMap[snd_name]->getFile().c_str(), -1));
  }

  // Set new filename
  if (objc == 3) {
	 try {
		if (!theSoundMap[snd_name]) {
		  theSoundMap[snd_name] = new HpAudioSound(theAudio, Tcl_GetString(objv[2]));
		}	 
		else {
		  theSoundMap[snd_name]->setFile(Tcl_GetString(objv[2]));
		}
	 }
	 catch (SoundError& err) {
 		err_message(interp, objv, err.msg().c_str());
		return TCL_ERROR;
	 }
  }
  return TCL_OK;
}

int SoundTcl::playCmd(ClientData, Tcl_Interp* interp,
							 int objc, Tcl_Obj* const objv[]) {
DOTRACE("SoundTcl::playCmd");
  if (objc != 2) {
	 Tcl_WrongNumArgs(interp, 1, objv, "sound_name");
	 return TCL_ERROR;
  }
  
  return playProc(interp, objv, Tcl_GetString(objv[1]));
}

int SoundTcl::playProc(Tcl_Interp* interp, Tcl_Obj* const objv[],
							  const char* snd_name) {
DOTRACE("SoundTcl::playProc");
  const char* err_info = 
	 (objv != NULL) ? Tcl_GetString(objv[0]) : "SoundTcl::playProc";

  if ( !theAudio ) {
	 err_message(interp, err_info, no_audio_msg);
	 return TCL_OK;
  }

  HpAudioSound* theSound = theSoundMap[snd_name];

  if (!theSound) {
	 if (objv != NULL)
	 err_message(interp, err_info, bad_sound_msg);
	 return TCL_ERROR;
  }

  theSound->play();
  return TCL_OK;
}

SoundTcl::SoundTcl(Tcl_Interp* interp) : itsStatus(TCL_OK) {
DOTRACE("SoundTcl::SoundTcl");
  // Add commands to ::Sound namespace.
  Tcl_CreateObjCommand(interp, "Sound::set", setCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "Sound::play", playCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);

  // If we had a successfull audio connection, set up some initial
  // sounds.
  if ( theAudio ) {
	 ASetCloseDownMode( theAudio, AKeepTransactions, NULL );

	 DebugEvalNL(AAudioString(theAudio));

	 // Try to initialize the default 'ok' and 'err' sounds.
	 // These will fail if the default sound files are missing.
	 try {
		theSoundMap["ok"] = new HpAudioSound(theAudio, ok_sound_file);
		theSoundMap["err"] = new HpAudioSound(theAudio, err_sound_file);
	 }
	 catch (SoundError& err) {
		Tcl_AppendStringsToObj(Tcl_GetObjResult(interp),
									  "Sound_Init: ", err.msg().c_str(), NULL);
		itsStatus = TCL_ERROR;
	 }
  }

  Tcl_PkgProvide(interp, "Sound", "1.1");
  itsStatus = TCL_OK;
}

SoundTcl::~SoundTcl() {
DOTRACE("SoundTcl::~SoundTcl");
  if ( theAudio ) {
	 ACloseAudio( theAudio, NULL );
	 theAudio = 0;
  }
}

//---------------------------------------------------------------------
//
// SoundPkg --
//
//---------------------------------------------------------------------

namespace SoundTcl_ {
  class SoundCmd;
  class HaveAudioCmd;
  class SoundPkg;
}

class SoundTcl_::SoundCmd : public TclCmd {
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

class SoundTcl_::HaveAudioCmd : public TclCmd {
public:
  HaveAudioCmd(Tcl_Interp* interp, const char* cmd_name) :
	 TclCmd(interp, cmd_name, NULL, 1, 1) {}
protected:
  virtual void invoke() { returnBool( theAudio != 0 ); }
};

class SoundTcl_::SoundPkg :
  public AbstractListItemPkg<HpAudioSound, SoundList> {
public:
  SoundPkg(Tcl_Interp* interp) :
	 AbstractListItemPkg<HpAudioSound, SoundList>(
			 interp, SoundList::theSoundList(), "Sound", "1.3")
  {
	 // Open an audio connection to the default audio server, then
	 // check to make sure connection succeeded. If the connection
	 // fails, 'audio' is set to NULL. In this case, all procedures in
	 // the HpAudioSound:: namespace will have no effect but will
	 // return without error.
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
		  Tcl_LinkInt(interp, "Sound::ok", &OK, TCL_LINK_READ_ONLY);

		  static int ERR = 1;
		  theList().insertAt(ERR, new HpAudioSound(theAudio, err_sound_file));
		  Tcl_LinkInt(interp, "Sound::err", &ERR, TCL_LINK_READ_ONLY);
		}
		catch (SoundError& err) {
		  Tcl_AppendStringsToObj(Tcl_GetObjResult(interp),
										 "SoundPkg: ", err.msg().c_str(), NULL);
		}

	 }
	 addCommand( new SoundCmd(interp, "Sound::Sound") );
	 addCommand( new HaveAudioCmd(interp, "Sound::haveAudio") );
	 declareAction("play", new CAction<Sound>(&Sound::play));
	 declareAttrib("file", new CAttrib<Sound, const string&>(&Sound::getFile,
																				&Sound::setFile));

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
  static SoundTcl aSoundTcl(interp);

  new SoundTcl_::SoundPkg(interp);
  new SoundListTcl::SoundListPkg(interp);

  return TCL_OK;
}

static const char vcid_soundtcl_cc[] = "$Header$";
#endif // !SOUNDTCL_CC_DEFINED
