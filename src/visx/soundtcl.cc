///////////////////////////////////////////////////////////////////////
// soundtcl.cc
// Rob Peters
// created: Tue Apr 13 14:09:59 1999
// written: Wed Apr 14 19:56:15 1999
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

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

///////////////////////////////////////////////////////////////////////
// Sound class declaration
///////////////////////////////////////////////////////////////////////

namespace {
  class SoundError {};
  class Sound {
  public:
	 Sound(Audio* audio, const string& filename);
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
// Sound Tcl package
///////////////////////////////////////////////////////////////////////

namespace SoundTcl {
  map< string, Sound* > theSoundMap;

  Tcl_ObjCmdProc setCmd;
  Tcl_ObjCmdProc playCmd;
  Tcl_ObjCmdProc set_ok_soundCmd;
  Tcl_ObjCmdProc set_err_soundCmd;
  Tcl_ObjCmdProc play_ok_soundCmd;
  Tcl_ObjCmdProc play_err_soundCmd;

  string ok_sound_file =  "/cit/rjpeters/face/audio/saw50_500Hz_300ms.au";
  string err_sound_file = "/cit/rjpeters/face/audio/saw50_200Hz_300ms.au";

  Audio *audio;

  // Error messages
  const char* bad_filename_msg = ": file does not exist";
  const char* bad_sound_msg = ": sound does not exist";
}

///////////////////////////////////////////////////////////////////////
// SoundTcl::Sound member definitions
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
	 throw SoundError();
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
// Sound Tcl package definitions
///////////////////////////////////////////////////////////////////////

int SoundTcl::setCmd(ClientData, Tcl_Interp *interp,
							int objc, Tcl_Obj *const objv[]) {
DOTRACE("SoundTcl::setCmd");
  Assert(audio);
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
	 catch (SoundError) {
 		err_message(interp, objv, bad_filename_msg);
		return TCL_ERROR;
	 }
  }
  return TCL_OK;
}

int SoundTcl::playCmd(ClientData, Tcl_Interp *interp,
							 int objc, Tcl_Obj *const objv[]) {
DOTRACE("SoundTcl::playCmd");
  Assert(audio);
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

int SoundTcl::Sound_Init(Tcl_Interp *interp) {
DOTRACE("SoundTcl::Sound_Init");
  const char *ServerName = "";
  audio = AOpenAudio( const_cast<char *>(ServerName), NULL );
  ASetCloseDownMode( audio, AKeepTransactions, NULL );

  theSoundMap["ok"] = new Sound(audio, ok_sound_file);
  theSoundMap["err"] = new Sound(audio, err_sound_file);

  // Add to ::Sound namespace
  Tcl_CreateObjCommand(interp, "Sound::set", setCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "Sound::play", playCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_PkgProvide(interp, "Sound", "1.1");
  return TCL_OK;
}

static const char vcid_soundtcl_cc[] = "$Header$";
#endif // !SOUNDTCL_CC_DEFINED
