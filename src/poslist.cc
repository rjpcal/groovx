///////////////////////////////////////////////////////////////////////
//
// poslist.cc
// Rob Peters
// created: Fri Mar 12 17:21:29 1999
// written: Fri Oct 27 17:51:29 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef POSLIST_CC_DEFINED
#define POSLIST_CC_DEFINED

#include "poslist.h"

#include "ioptrlist.h"

#define NO_TRACE
#include "util/trace.h"

IoPtrList& PosList::thePosList() {
DOTRACE("PosList::thePosList");
  return IoPtrList::theList();
}

///////////////////////////////////////////////////////////////////////
//
// Template instantiation of PtrList<Position>
//
///////////////////////////////////////////////////////////////////////

#include "position.h"

template <>
IoPtrList& IdItem<Position>::ptrList()
{ return PosList::thePosList(); }

template <>
IoPtrList& MaybeIdItem<Position>::ptrList()
{ return PosList::thePosList(); }

static const char vcid_poslist_cc[] = "$Header$";
#endif // !POSLIST_CC_DEFINED
