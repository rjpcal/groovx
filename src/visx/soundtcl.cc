///////////////////////////////////////////////////////////////////////
//
// soundtcl.cc
// Rob Peters
// created: Tue Apr 13 14:09:59 1999
// written: Thu Feb 17 15:04:16 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef SOUNDTCL_CC_DEFINED
#define SOUNDTCL_CC_DEFINED

#include <tcl.h>
#include <string>

#include "soundlist.h"
#include "sound.h"
#include "listpkg.h"
#include "listitempkg.h"
#include "tcllink.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

//---------------------------------------------------------------------
//
// SoundTcl namespace --
//
//---------------------------------------------------------------------

namespace SoundTcl {
  string ok_sound_file = "/cit/rjpeters/face/audio/saw50_500Hz_300ms.au";
  string err_sound_file = "/cit/rjpeters/face/audio/saw50_350Hz_300ms.au";

  class SoundCmd;
  class HaveAudioCmd;
  class SoundPkg;
}

//---------------------------------------------------------------------
//
// SoundTcl::SoundCmd --
//
//---------------------------------------------------------------------

class SoundTcl::SoundCmd : public Tcl::TclCmd {
public:
  SoundCmd(Tcl_Interp* interp, const char* cmd_name) :
	 Tcl::TclCmd(interp, cmd_name, "filename", 2, 2) {}
protected:
  virtual void invoke() {
	 const char* filename = getCstringFromArg(1);

	 Sound* p = Sound::newPlatformSound(filename);
	 int id = SoundList::theSoundList().insert(SoundList::Ptr(p));
	 returnInt(id);
  }
};

//---------------------------------------------------------------------
//
// SoundTcl::HaveAudioCmd --
//
//---------------------------------------------------------------------

class SoundTcl::HaveAudioCmd : public Tcl::TclCmd {
public:
  HaveAudioCmd(Tcl_Interp* interp, const char* cmd_name) :
	 Tcl::TclCmd(interp, cmd_name, NULL, 1, 1) {}
protected:
  virtual void invoke() { returnBool( Sound::haveSound() ); }
};

//---------------------------------------------------------------------
//
// SoundTcl::SoundPkg --
//
//---------------------------------------------------------------------

class SoundTcl::SoundPkg :
  public Tcl::AbstractListItemPkg<Sound, SoundList> {
public:
  SoundPkg(Tcl_Interp* interp) :
	 Tcl::AbstractListItemPkg<Sound, SoundList>(
			 interp, SoundList::theSoundList(), "Sound", "1.3")
  {
	 bool haveSound = Sound::initSound();

	 if (!haveSound) {
		Tcl_AppendStringsToObj(Tcl_GetObjResult(interp),
									  "SoundPkg: couldn't initialize sound system",
									  NULL);
	 }
	 else {
		try {
		  static int OK = 0;
		  theList().insertAt(OK, SoundList::Ptr(
										    Sound::newPlatformSound(ok_sound_file)));
		  linkConstVar("Sound::ok", OK);

		  static int ERR = 1;
		  theList().insertAt(ERR, SoundList::Ptr(
			    							 Sound::newPlatformSound(err_sound_file)));
		  linkConstVar("Sound::err", ERR);
		}
		catch (SoundError& err) {
		  DebugPrintNL("error creating sounds during startup");
		  Tcl_AppendStringsToObj(Tcl_GetObjResult(interp),
										 "SoundPkg: ", err.msg().c_str(), NULL);
		}
	 }

	 addCommand( new SoundCmd(interp, "Sound::Sound") );
	 addCommand( new HaveAudioCmd(interp, "Sound::haveAudio") );
	 declareCAction("play", &Sound::play);
	 declareCAttrib("file", &Sound::getFile, &Sound::setFile);
  }

  ~SoundPkg() {
	 Sound::closeSound();
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

class SoundListTcl::SoundListPkg : public Tcl::IoPtrListPkg {
public:
  SoundListPkg(Tcl_Interp* interp) :
	 Tcl::IoPtrListPkg(interp, SoundList::theSoundList(),
							 "SoundList", "1.3") {}
};

//---------------------------------------------------------------------
//
// Sound_Init --
//
//---------------------------------------------------------------------

extern "C" Tcl_PackageInitProc Sound_Init;

int Sound_Init(Tcl_Interp* interp) {
DOTRACE("Sound_Init");

  new SoundTcl::SoundPkg(interp);
  new SoundListTcl::SoundListPkg(interp);

  return TCL_OK;
}

static const char vcid_soundtcl_cc[] = "$Header$";
#endif // !SOUNDTCL_CC_DEFINED
