///////////////////////////////////////////////////////////////////////
//
// sound.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Jul  8 11:43:07 1999
// written: Thu Oct 14 11:04:38 1999
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

Sound::~Sound () {}

static const char vcid_sound_cc[] = "$Header$";
#endif // !SOUND_CC_DEFINED
