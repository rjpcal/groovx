///////////////////////////////////////////////////////////////////////
//
// soundlist.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Jul  8 11:44:41 1999
// written: Thu Jul  8 11:48:39 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef SOUNDLIST_CC_DEFINED
#define SOUNDLIST_CC_DEFINED

#include "soundlist.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

namespace {
  SoundList* instance = 0;
  const int DEFAULT_INIT_SIZE = 10;
}

SoundList::SoundList(int size) : Base(size) {
DOTRACE("SoundList::SoundList");
  // nothing
}

SoundList& SoundList::theSoundList() {
DOTRACE("SoundList::theSoundList");
  if (instance == 0) {
	 instance = new SoundList(DEFAULT_INIT_SIZE);
  }
  return *instance;
}

///////////////////////////////////////////////////////////////////////
//
// Template instantiation of PtrList<Sound>
//
///////////////////////////////////////////////////////////////////////

#include "sound.h"
#include "ptrlist.cc"
template class PtrList<Sound>;

static const char vcid_soundlist_cc[] = "$Header$";
#endif // !SOUNDLIST_CC_DEFINED
