///////////////////////////////////////////////////////////////////////
//
// soundtcl.cc
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Apr 13 14:09:59 1999
// written: Mon Dec 11 14:29:48 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef SOUNDTCL_CC_DEFINED
#define SOUNDTCL_CC_DEFINED

#include "application.h"
#include "sound.h"

#include "io/iofactory.h"

#include "tcl/ioitempkg.h"
#include "tcl/listpkg.h"
#include "tcl/tcllink.h"

#include "util/strings.h"

#include <tcl.h>

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

//---------------------------------------------------------------------
//
// SoundTcl namespace --
//
//---------------------------------------------------------------------

namespace SoundTcl {
  const char* ok_sound_file = "/audio/saw50_500Hz_300ms.au";
  const char* err_sound_file = "/audio/saw50_350Hz_300ms.au";

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
	 IdItem<Sound> sound(p, IdItem<Sound>::Insert());
	 returnInt(sound.id());
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
  public Tcl::IoItemPkg<Sound> {
public:
  SoundPkg(Tcl_Interp* interp) :
	 Tcl::IoItemPkg<Sound>(interp, "Sound", "$Revision$")
  {
	 bool haveSound = Sound::initSound();

	 if (!haveSound) {
		Tcl_AppendStringsToObj(Tcl_GetObjResult(interp),
									  "SoundPkg: couldn't initialize sound system",
									  NULL);
	 }
	 else {
		dynamic_string lib_dir(Application::theApp().getLibraryDirectory());
		DebugEvalNL(lib_dir);

		dynamic_string full_ok_file(lib_dir);
		full_ok_file.append(ok_sound_file);     DebugEvalNL(full_ok_file);

		dynamic_string full_err_file(lib_dir);
		full_err_file.append(err_sound_file);     DebugEvalNL(full_err_file);

		static int OK = -1;
		static int ERR = -1;

		try {
		  IdItem<Sound> ok_sound(
               Sound::newPlatformSound(full_ok_file.c_str()),
					IdItem<Sound>::Insert());
		  Sound::setOkSound(ok_sound);
		  OK = ok_sound.id();
		  linkConstVar("Sound::ok", OK);

		  IdItem<Sound> err_sound(
               Sound::newPlatformSound(full_err_file.c_str()),
					IdItem<Sound>::Insert());
		  Sound::setErrSound(err_sound);
		  ERR = err_sound.id();		  
		  linkConstVar("Sound::err", ERR);
		}
		catch (SoundError& err) {
		  DebugPrintNL("error creating sounds during startup");
		  Tcl_AppendStringsToObj(Tcl_GetObjResult(interp),
										 "SoundPkg: ", err.msg_cstr(), NULL);
		  setInitStatusError();
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

class SoundListTcl::SoundListPkg : public Tcl::PtrListPkg<Sound> {
public:
  SoundListPkg(Tcl_Interp* interp) :
	 Tcl::PtrListPkg<Sound>(interp, "SoundList", "$Revision$") {}
};

//---------------------------------------------------------------------
//
// Sound_Init --
//
//---------------------------------------------------------------------

extern "C" int Sound_Init(Tcl_Interp* interp) {
DOTRACE("Sound_Init");

  Tcl::TclPkg* pkg1 = new SoundTcl::SoundPkg(interp);
  Tcl::TclPkg* pkg2 = new SoundListTcl::SoundListPkg(interp);

  IO::IoFactory::theOne().registerCreatorFunc(&Sound::make);

  return pkg1->initedOk() ? pkg2->initStatus() : pkg1->initStatus();
}

static const char vcid_soundtcl_cc[] = "$Header$";
#endif // !SOUNDTCL_CC_DEFINED
