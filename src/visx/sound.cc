///////////////////////////////////////////////////////////////////////
//
// sound.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Jul  8 11:43:07 1999
// written: Wed Jun  6 15:53:07 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef SOUND_CC_DEFINED
#define SOUND_CC_DEFINED

#include "sound.h"

#include "util/iditem.h"

#if defined(HP9000S700)
#  include "hpsound.h"
#elif defined(IRIX6)
#  include "irixsound.h"
#elif defined(I686)
#  include "esdsound.h"
#else
#  include "dummysound.h"
#endif

namespace {
  MaybeIdItem<Sound> OK_SOUND;
  MaybeIdItem<Sound> ERR_SOUND;
}

SoundError::SoundError() : ErrorWithMsg() {}
SoundError::SoundError(const char* msg) : ErrorWithMsg(msg) {}
SoundError::~SoundError() {}

Sound::~Sound () {}

void Sound::setOkSound(IdItem<Sound> ok_sound) {
#ifndef ACC_COMPILER
  OK_SOUND = ok_sound;
#else
  OK_SOUND = MaybeIdItem<Sound>(ok_sound);
#endif
}

void Sound::setErrSound(IdItem<Sound> err_sound) {
#ifndef ACC_COMPILER
  ERR_SOUND = err_sound;
#else
  ERR_SOUND = MaybeIdItem<Sound>(err_sound);
#endif
}

IdItem<Sound> Sound::getOkSound() {
  return IdItem<Sound>(OK_SOUND.get());
}

IdItem<Sound> Sound::getErrSound() {
  return IdItem<Sound>(ERR_SOUND.get());
}

static const char vcid_sound_cc[] = "$Header$";
#endif // !SOUND_CC_DEFINED
