///////////////////////////////////////////////////////////////////////
//
// soundtcl.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Apr 13 14:09:59 1999
// written: Wed Sep 11 14:56:47 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef SOUNDTCL_CC_DEFINED
#define SOUNDTCL_CC_DEFINED

#include "grsh/grsh.h"

#include "tcl/tclpkg.h"
#include "tcl/tclsafeinterp.h"

#include "util/error.h"
#include "util/objfactory.h"
#include "util/strings.h"

#include "visx/sound.h"

#include "util/trace.h"
#include "util/debug.h"

//---------------------------------------------------------------------
//
// SoundTcl namespace --
//
//---------------------------------------------------------------------

namespace SoundTcl
{
  const char* ok_sound_file = "/audio/saw50_500Hz_300ms.au";
  const char* err_sound_file = "/audio/saw50_350Hz_300ms.au";

  class SoundPkg;
}

//---------------------------------------------------------------------
//
// SoundTcl::SoundPkg --
//
//---------------------------------------------------------------------

class SoundTcl::SoundPkg : public Tcl::Pkg
{
public:
  SoundPkg(const Tcl::Interp& interp) :
    Tcl::Pkg(interp.intp(), "Sound", "$Revision$")
  {
    Tcl::defGenericObjCmds<Sound>(this);

    bool haveSound = Sound::initSound();

    if (!haveSound)
      {
        interp.appendResult("SoundPkg: couldn't initialize sound system");
        setInitStatusError();
      }
    else
      {
        fstring lib_dir(Grsh::libraryDirectory());
        DebugEvalNL(lib_dir);

        fstring full_ok_file(lib_dir);
        full_ok_file.append(ok_sound_file);     DebugEvalNL(full_ok_file);

        fstring full_err_file(lib_dir);
        full_err_file.append(err_sound_file);     DebugEvalNL(full_err_file);

        static int OK = -1;
        static int ERR = -1;

        try
          {
            Ref<Sound> ok_sound(Sound::newPlatformSound(full_ok_file.c_str()));
            Sound::setOkSound(ok_sound);
            OK = ok_sound.id();
            linkConstVar("Sound::ok", OK);

            Ref<Sound> err_sound(Sound::newPlatformSound(full_err_file.c_str()));
            Sound::setErrSound(err_sound);
            ERR = err_sound.id();
            linkConstVar("Sound::err", ERR);
          }
        catch (Util::Error& err)
          {
            DebugPrintNL("error creating sounds during startup");
            interp.appendResult("SoundPkg: ");
            interp.appendResult(err.msg_cstr());
            setInitStatusError();
          }
      }

    def( "haveAudio", 0, &Sound::haveSound );

    defAction("play", &Sound::play);
    defAttrib("file", &Sound::getFile, &Sound::setFile);
  }

  ~SoundPkg()
  {
    Sound::closeSound();
  }
};

//---------------------------------------------------------------------
//
// Sound_Init --
//
//---------------------------------------------------------------------

extern "C" int Sound_Init(Tcl_Interp* interp)
{
DOTRACE("Sound_Init");

  Tcl::Interp intp(interp);
  Tcl::Pkg* pkg = new SoundTcl::SoundPkg(intp);

  Util::ObjFactory::theOne().registerCreatorFunc(&Sound::make);

  return pkg->initStatus();
}

static const char vcid_soundtcl_cc[] = "$Header$";
#endif // !SOUNDTCL_CC_DEFINED
