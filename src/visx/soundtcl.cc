///////////////////////////////////////////////////////////////////////
//
// soundtcl.cc
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Tue Apr 13 14:09:59 1999
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef SOUNDTCL_CC_DEFINED
#define SOUNDTCL_CC_DEFINED

#include "system/system.h"

#include "tcl/objpkg.h"
#include "tcl/tclpkg.h"
#include "tcl/tclsafeinterp.h"

#include "util/objfactory.h"
#include "util/strings.h"

#include "visx/sound.h"

#include <exception>

#include "util/trace.h"
#include "util/debug.h"

class SoundPkg : public Tcl::Pkg
{
public:
  SoundPkg(const Tcl::Interp& interp) :
    Tcl::Pkg(interp.intp(), "Sound", "$Revision$")
  {
    this->inheritPkg("IO");
    Tcl::defGenericObjCmds<Sound>(this);

    bool haveSound = Sound::initSound();

    if (!haveSound)
      {
        interp.appendResult("SoundPkg: couldn't initialize sound system");
        setInitStatusError();
      }
    else
      {
        fstring lib_dir(System::theSystem().getenv("GRSH_LIB_DIR"));
        dbgEvalNL(3, lib_dir);

        const fstring ok_file(lib_dir, "/audio/saw50_500Hz_300ms.au");
        const fstring err_file(lib_dir, "/audio/saw50_350Hz_300ms.au");

        static int OK = -1;
        static int ERR = -1;

        try
          {
            Ref<Sound> ok_sound(Sound::makeFrom(ok_file.c_str()));
            Sound::setOkSound(ok_sound);
            OK = ok_sound.id();
            linkConstVar("Sound::ok", OK);

            Ref<Sound> err_sound(Sound::makeFrom(err_file.c_str()));
            Sound::setErrSound(err_sound);
            ERR = err_sound.id();
            linkConstVar("Sound::err", ERR);
          }
        catch (std::exception& err)
          {
            dbgPrintNL(3, "error creating sounds during startup");
            interp.appendResult("SoundPkg: ");
            interp.appendResult(err.what());
            setInitStatusError();
          }
      }

    def( "haveAudio", 0, &Sound::haveSound );

    defAction("play", &Sound::play);
    defAction("forceLoad", &Sound::forceLoad);
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
  Tcl::Pkg* pkg = new SoundPkg(intp);

  Util::ObjFactory::theOne().registerCreatorFunc(&Sound::make);

  return pkg->initStatus();
}

static const char vcid_soundtcl_cc[] = "$Header$";
#endif // !SOUNDTCL_CC_DEFINED
