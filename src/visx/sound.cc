///////////////////////////////////////////////////////////////////////
//
// sound.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Jul  8 11:43:07 1999
// written: Sat Sep  8 08:56:00 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef SOUND_CC_DEFINED
#define SOUND_CC_DEFINED

#include "sound.h"

#include "util/ref.h"

#if defined(HP9000S700)
#  include "hpsound.h"
#elif defined(IRIX6)
#  include "irixsound.h"
#elif defined(I686)
#  include "esdsound.h"
#else
#  include "dummysound.h"
#endif

namespace
{
  SoftRef<Sound> OK_SOUND;
  SoftRef<Sound> ERR_SOUND;
}

Sound::~Sound () {}

void Sound::setOkSound(Ref<Sound> ok_sound)
{
  OK_SOUND = ok_sound;
}

void Sound::setErrSound(Ref<Sound> err_sound)
{
  ERR_SOUND = err_sound;
}

Ref<Sound> Sound::getOkSound()
{
  return Ref<Sound>(OK_SOUND.get());
}

Ref<Sound> Sound::getErrSound()
{
  return Ref<Sound>(ERR_SOUND.get());
}

static const char vcid_sound_cc[] = "$Header$";
#endif // !SOUND_CC_DEFINED
