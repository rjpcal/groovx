///////////////////////////////////////////////////////////////////////
//
// sound.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Jul  8 11:43:07 1999
// written: Fri May 11 21:18:01 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef SOUND_CC_DEFINED
#define SOUND_CC_DEFINED

#include "sound.h"

#include "io/iditem.h"

#ifdef HP9000S700
#include "hpsound.h"
#endif

#ifdef IRIX6
#include "irixsound.h"
#endif

#ifdef I686
#include "intelsound.h"
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
  OK_SOUND = ok_sound;
}

void Sound::setErrSound(IdItem<Sound> err_sound) {
  ERR_SOUND = err_sound;
}

IdItem<Sound> Sound::getOkSound() {
  return IdItem<Sound>(OK_SOUND.get());
}

IdItem<Sound> Sound::getErrSound() {
  return IdItem<Sound>(ERR_SOUND.get());
}

static const char vcid_sound_cc[] = "$Header$";
#endif // !SOUND_CC_DEFINED
