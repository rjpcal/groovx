///////////////////////////////////////////////////////////////////////
// soundtcl.cc
// Rob Peters
// created: Tue Apr 13 14:09:59 1999
// written: Tue Apr 27 13:29:29 1999
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

#define LOCAL_PROF
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

///////////////////////////////////////////////////////////////////////
// Sound class declaration
///////////////////////////////////////////////////////////////////////

namespace {
  class SoundError {
  public:
	 SoundError() : itsInfo() {}
	 SoundError(const string& filename) { setInfo(filename); }
	 const string& info() { return itsInfo; }
  protected:
	 void setInfo(const string& filename) {
		itsInfo = string("SoundError: bad or nonexistent file '") + filename + "'";
	 }
  private:
	 string itsInfo;
  };

  class Sound {
  public:
	 Sound(Audio* audio, const string& filename);
	 virtual ~Sound();
	 void play();
	 void setFile(const string& filename);
	 const string& getFile() const { return itsFilename; }
  private:
	 Audio* itsAudio;
	 string itsFilename;
	 SBucket* itsSBucket;
	 SBPlayParams itsPlayParams;
  };
}

///////////////////////////////////////////////////////////////////////
// Sound member definitions
///////////////////////////////////////////////////////////////////////

Sound::Sound(Audio* audio, const string& filename) :
  itsAudio(audio), itsSBucket(NULL)
{
DOTRACE("Sound::Sound");
  Assert(itsAudio);

  setFile(filename);

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
}

Sound::~Sound() {
DOTRACE("Sound::~Sound");
  ADestroySBucket( itsAudio, itsSBucket, NULL );
}

void Sound::play() {
DOTRACE("Sound::play");
  Assert(itsAudio);
  ATransID xid = APlaySBucket( itsAudio, itsSBucket, &itsPlayParams, NULL );
}
	
void Sound::setFile(const string& filename) {
DOTRACE("Sound::setFile");
  Assert(itsAudio);

  ifstream ifs(filename.c_str());
  if (ifs.fail()) {
	 throw SoundError(filename);
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
  map< string, Sound* > theSoundMap;

  string ok_sound_file = "/cit/rjpeters/face/audio/saw50_500Hz_300ms.au";
  string err_sound_file = "/cit/rjpeters/face/audio/saw50_350Hz_300ms.au";

  Audio* audio;

  // Error messages
  const char* const bad_filename_msg = "sound file does not exist";
  const char* const bad_sound_msg = "sound does not exist";
  const char* const no_audio_msg = "audio is not enabled";
}

///////////////////////////////////////////////////////////////////////
// Sound Tcl package definitions
///////////////////////////////////////////////////////////////////////

int SoundTcl::setCmd(ClientData, Tcl_Interp *interp,
							int objc, Tcl_Obj *const objv[]) {
DOTRACE("SoundTcl::setCmd");
  if ( !audio ) {
	 err_message(interp, objv, no_audio_msg);
	 return TCL_OK;
  }

  if (objc < 2 || objc > 3) {
	 Tcl_WrongNumArgs(interp, 1, objv, "sound_name [new_filename]");
	 return TCL_ERROR;
  }
  
  const char* sound = Tcl_GetString(objv[1]);

  if (objc == 2) {
	 if (!theSoundMap[sound]) {
		err_message(interp, objv, bad_sound_msg);
		return TCL_ERROR;
	 }
	 Tcl_SetObjResult(interp,
		    Tcl_NewStringObj(theSoundMap[sound]->getFile().c_str(), -1));
  }
  if (objc == 3) {
	 try {
		if (!theSoundMap[sound]) {
		  theSoundMap[sound] = new Sound(audio, Tcl_GetString(objv[2]));
		}	 
		else {
		  theSoundMap[sound]->setFile(Tcl_GetString(objv[2]));
		}
	 }
	 catch (SoundError& err) {
 		err_message(interp, objv, err.info().c_str());
		return TCL_ERROR;
	 }
  }
  return TCL_OK;
}

int SoundTcl::playCmd(ClientData, Tcl_Interp *interp,
							 int objc, Tcl_Obj *const objv[]) {
DOTRACE("SoundTcl::playCmd");
  if ( !audio ) {
	 err_message(interp, objv, no_audio_msg);
	 return TCL_OK;
  }

  if (objc != 2) {
	 Tcl_WrongNumArgs(interp, 1, objv, "sound_name");
	 return TCL_ERROR;
  }
  
  const char* snd_name = Tcl_GetString(objv[1]);

  if (!theSoundMap[snd_name]) {
	 err_message(interp, objv, bad_sound_msg);
	 return TCL_ERROR;
  }

  theSoundMap[snd_name]->play();
  return TCL_OK;
}

int SoundTcl::haveAudioCmd(ClientData, Tcl_Interp *interp,
									int objc, Tcl_Obj *const objv[]) {
DOTRACE("SoundTcl::haveAudioCmd");
  if ( !audio ) {
	 Tcl_SetObjResult(interp, Tcl_NewIntObj(0));
  }
  else {
	 Tcl_SetObjResult(interp, Tcl_NewIntObj(1));
  }
  return TCL_OK;
}

SoundTcl::SoundTcl(Tcl_Interp *interp) : itsStatus(TCL_OK) {
DOTRACE("SoundTcl::SoundTcl");
  // Open an audio connection to the default audio server, then check
  // to make sure connection succeeded. If the connection fails,
  // 'audio' is set to NULL. In this case, all procedures in the
  // Sound:: namespace will have no effect but will return without
  // error.
  const char* ServerName = "";
  long status = 0;
  audio = AOpenAudio( const_cast<char *>(ServerName), &status );
  if ( status != 0 ) {
	 audio = NULL;
  }

  // Add commands to ::Sound namespace.
  Tcl_CreateObjCommand(interp, "Sound::set", setCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "Sound::play", playCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "Sound::haveAudio", haveAudioCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);

  // If we had a successfull audio connection, set up some initial
  // sounds.
  if ( audio ) {
	 ASetCloseDownMode( audio, AKeepTransactions, NULL );
#ifdef LOCAL_DEBUG
	 DUMP_VAL2(AAudioString(audio));
#endif

	 // Try to initialize the default 'ok' and 'err' sounds.
	 // These will fail if the default sound files are missing.
	 try {
		theSoundMap["ok"] = new Sound(audio, ok_sound_file);
		theSoundMap["err"] = new Sound(audio, err_sound_file);
	 }
	 catch (SoundError& err) {
		Tcl_AppendStringsToObj(Tcl_GetObjResult(interp),
									  "Sound_Init: ", err.info().c_str(), NULL);
		itsStatus = TCL_ERROR;
	 }
  }

  Tcl_PkgProvide(interp, "Sound", "1.1");
  itsStatus = TCL_OK;
}

SoundTcl::~SoundTcl() {
DOTRACE("SoundTcl::~SoundTcl");
  if ( audio ) {
	 ACloseAudio( audio, NULL );
  }
}

static const char vcid_soundtcl_cc[] = "$Header$";
#endif // !SOUNDTCL_CC_DEFINED
