///////////////////////////////////////////////////////////////////////
//
// soundlist.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Jul  8 11:44:41 1999
// written: Thu Oct 26 17:52:56 2000
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
PtrList<Sound>& IdItem<Sound>::ptrList()
{ return SoundList::theSoundList(); }

template <>
PtrList<Sound>& MaybeIdItem<Sound>::ptrList()
{ return SoundList::theSoundList(); }

#include "iditem.cc"
template class IdItem<Sound>;
template class MaybeIdItem<Sound>;

static const char vcid_soundlist_cc[] = "$Header$";
#endif // !SOUNDLIST_CC_DEFINED
