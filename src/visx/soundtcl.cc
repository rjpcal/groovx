///////////////////////////////////////////////////////////////////////
//
// soundtcl.cc
//
// Copyright (c) 1999-2004 Rob Peters rjpeters at klab dot caltech dot edu
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

#include "tcl/objpkg.h"
#include "tcl/tclpkg.h"
#include "tcl/tclsafeinterp.h"

#include "util/objfactory.h"
#include "util/strings.h"

#include "visx/sound.h"

#include <exception>

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER

extern "C"
int Sound_Init(Tcl_Interp* interp)
{
DOTRACE("Sound_Init");

  Tcl::Interp intp(interp);
  PKG_CREATE(interp, "Sound", "$Revision$");

  pkg->onExit(&Sound::closeSound);

  pkg->inheritPkg("IO");
  Tcl::defGenericObjCmds<Sound>(pkg);

  Util::ObjFactory::theOne().registerCreatorFunc(&Sound::make);

  pkg->def( "haveAudio", 0, &Sound::haveSound );

  pkg->defAction("play", &Sound::play);
  pkg->defAction("forceLoad", &Sound::forceLoad);
  pkg->defAttrib("file", &Sound::getFile, &Sound::setFile);

  bool haveSound = Sound::initSound();

  if (!haveSound)
    {
      intp.appendResult("SoundPkg: couldn't initialize sound system");
      pkg->setInitStatusError();
    }
  else
    {
      const fstring ok_file(GROOVX_AUDIO_DIR "/saw50_500Hz_300ms.au");
      const fstring err_file(GROOVX_AUDIO_DIR "/saw50_350Hz_2x120ms.au");

      static int OK = -1;
      static int ERR = -1;

      try
        {
          Util::Ref<Sound> ok_sound(Sound::makeFrom(ok_file.c_str()));
          Sound::setOkSound(ok_sound);
          OK = ok_sound.id();
          pkg->linkConstVar("Sound::ok", OK);

          Util::Ref<Sound> err_sound(Sound::makeFrom(err_file.c_str()));
          Sound::setErrSound(err_sound);
          ERR = err_sound.id();
          pkg->linkConstVar("Sound::err", ERR);
        }
      catch (std::exception& err)
        {
          dbgPrintNL(3, "error creating sounds during startup");
          intp.appendResult("SoundPkg: ");
          intp.appendResult(err.what());
          pkg->setInitStatusError();
        }
    }

  PKG_RETURN;
}

static const char vcid_soundtcl_cc[] = "$Header$";
#endif // !SOUNDTCL_CC_DEFINED
