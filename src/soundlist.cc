///////////////////////////////////////////////////////////////////////
//
// soundlist.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Jul  8 11:44:41 1999
// written: Sun Oct  8 15:59:33 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef SOUNDLIST_CC_DEFINED
#define SOUNDLIST_CC_DEFINED

#include "soundlist.h"

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

namespace {
  const int DEFAULT_INIT_SIZE = 10;
}

SoundList::SoundList(int size) : Base(size) {
DOTRACE("SoundList::SoundList");
  // nothing
}

SoundList::~SoundList() {}

SoundList SoundList::theInstance(DEFAULT_INIT_SIZE);

SoundList& SoundList::theSoundList() {
DOTRACE("SoundList::theSoundList");
  return theInstance; 
}

///////////////////////////////////////////////////////////////////////
//
// Template instantiation of PtrList<Sound>
//
///////////////////////////////////////////////////////////////////////

#include "sound.h"
#include "ptrlist.cc"
template class MasterPtr<Sound>;
template class PtrList<Sound>;

static const char vcid_soundlist_cc[] = "$Header$";
#endif // !SOUNDLIST_CC_DEFINED
