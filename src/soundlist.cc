///////////////////////////////////////////////////////////////////////
//
// soundlist.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Jul  8 11:44:41 1999
// written: Fri Oct 27 17:50:43 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef SOUNDLIST_CC_DEFINED
#define SOUNDLIST_CC_DEFINED

#include "soundlist.h"

#include "ioptrlist.h"

#define NO_TRACE
#include "util/trace.h"

IoPtrList& SoundList::theSoundList() {
DOTRACE("SoundList::theSoundList");
  return IoPtrList::theList(); 
}

///////////////////////////////////////////////////////////////////////
//
// Template instantiation of PtrList<Sound>
//
///////////////////////////////////////////////////////////////////////

#include "sound.h"

template <>
IoPtrList& IdItem<Sound>::ptrList()
{ return SoundList::theSoundList(); }

template <>
IoPtrList& MaybeIdItem<Sound>::ptrList()
{ return SoundList::theSoundList(); }

static const char vcid_soundlist_cc[] = "$Header$";
#endif // !SOUNDLIST_CC_DEFINED
