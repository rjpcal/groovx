/** @file visx/tclpkg-sound.cc tcl interface package for class Sound */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Tue Apr 13 14:09:59 1999
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [https://github.com/rjpcal/groovx]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#include "visx/tclpkg-sound.h"

#include "nub/objfactory.h"

#include "tcl/objpkg.h"
#include "tcl/pkg.h"
#include "tcl/interp.h"

#include "rutz/fstring.h"

#include "visx/sound.h"

#include <exception>

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

extern "C"
int Sound_Init(Tcl_Interp* interp)
{
GVX_TRACE("Sound_Init");

  tcl::interpreter intp(interp);
  return tcl::pkg::init
    (interp, "Sound", "4.0",
     [](tcl::pkg* pkg) {

      pkg->inherit_pkg("io");
      tcl::def_basic_type_cmds<Sound>(pkg, SRC_POS);
      tcl::def_creator<Sound>(pkg);

      pkg->def_action("play", &Sound::play, SRC_POS);
      pkg->def_action("forceLoad", &Sound::forceLoad, SRC_POS);
      pkg->def_get_set("file", &Sound::getFile, &Sound::setFile, SRC_POS);

      const rutz::fstring ok_file(GROOVX_AUDIO_DIR "/saw50_500Hz_300ms.au");
      const rutz::fstring err_file(GROOVX_AUDIO_DIR "/saw50_350Hz_2x120ms.au");

      static int OK = -1;
      static int ERR = -1;

      nub::ref<Sound> ok_sound(Sound::makeFrom(ok_file.c_str()));
      Sound::setOkSound(ok_sound);
      GVX_ASSERT(ok_sound.id() < std::numeric_limits<int>::max());
      OK = int(ok_sound.id());
      pkg->link_var_const("Sound::ok", OK);

      nub::ref<Sound> err_sound(Sound::makeFrom(err_file.c_str()));
      Sound::setErrSound(err_sound);
      GVX_ASSERT(err_sound.id() < std::numeric_limits<int>::max());
      ERR = int(err_sound.id());
      pkg->link_var_const("Sound::err", ERR);
    });
}
