///////////////////////////////////////////////////////////////////////
//
// sound.cc
//
// Copyright (c) 1999-2003 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Jul  8 11:43:07 1999
// written: Mon Jan 13 11:08:25 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef SOUND_CC_DEFINED
#define SOUND_CC_DEFINED

#include "visx/sound.h"

#include "util/ref.h"

#if defined(HAVE_ALIB_H)
#  include "visx/hpsound.h"
#elif defined(HAVE_DMEDIA_AUDIO_H)
#  include "visx/irixsound.h"
#elif defined(HAVE_ESD_H)
#  include "visx/esdsound.h"
#else
#  include "visx/dummysound.h"
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
