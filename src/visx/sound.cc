///////////////////////////////////////////////////////////////////////
//
// sound.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Jul  8 11:43:07 1999
// written: Thu Jun  1 13:57:41 2000
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

SoundError::SoundError() : ErrorWithMsg() {}
SoundError::SoundError(const char* msg) : ErrorWithMsg(msg) {}
SoundError::~SoundError() {}

Sound::~Sound () {}

static const char vcid_sound_cc[] = "$Header$";
#endif // !SOUND_CC_DEFINED
