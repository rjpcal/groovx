///////////////////////////////////////////////////////////////////////
//
// sound.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Jul  8 11:43:07 1999
// written: Tue Oct 24 13:01:42 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef SOUND_CC_DEFINED
#define SOUND_CC_DEFINED

#include "sound.h"

#ifdef HP9000S700
#include "hpsound.cc"
#endif

#ifdef IRIX6
#include "irixsound.cc"
#endif

namespace {
  NullableItemWithId<Sound> OK_SOUND(0);
  NullableItemWithId<Sound> ERR_SOUND(0);
}

SoundError::SoundError() : ErrorWithMsg() {}
SoundError::SoundError(const char* msg) : ErrorWithMsg(msg) {}
SoundError::~SoundError() {}

Sound::~Sound () {}

void Sound::setOkSound(ItemWithId<Sound> ok_sound) {
  OK_SOUND = ok_sound;
}

void Sound::setErrSound(ItemWithId<Sound> err_sound) {
  ERR_SOUND = err_sound;
}

ItemWithId<Sound> Sound::getOkSound() {
  return ItemWithId<Sound>(OK_SOUND.get(), OK_SOUND.id());
}

ItemWithId<Sound> Sound::getErrSound() {
  return ItemWithId<Sound>(ERR_SOUND.get(), ERR_SOUND.id());
}

static const char vcid_sound_cc[] = "$Header$";
#endif // !SOUND_CC_DEFINED
