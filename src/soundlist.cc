///////////////////////////////////////////////////////////////////////
//
// soundlist.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Jul  8 11:44:41 1999
// written: Wed Oct 25 17:52:35 2000
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

SoundList::SoundList() : Base() {
DOTRACE("SoundList::SoundList");
  // nothing
}

SoundList::~SoundList() {}

SoundList SoundList::theInstance;

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
template class PtrList<Sound>;

template <>
PtrList<Sound>& ItemWithId<Sound>::ptrList()
{ return SoundList::theSoundList(); }

template <>
PtrList<Sound>& NullableItemWithId<Sound>::ptrList()
{ return SoundList::theSoundList(); }

#include "itemwithid.cc"
template class ItemWithId<Sound>;
template class NullableItemWithId<Sound>;

static const char vcid_soundlist_cc[] = "$Header$";
#endif // !SOUNDLIST_CC_DEFINED
