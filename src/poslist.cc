///////////////////////////////////////////////////////////////////////
//
// poslist.cc
// Rob Peters
// created: Fri Mar 12 17:21:29 1999
// written: Wed Oct 25 17:52:11 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef POSLIST_CC_DEFINED
#define POSLIST_CC_DEFINED

#include "poslist.h"

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

PosList PosList::theInstance;

PosList::PosList() : Base()
{
DOTRACE("PosList::PosList");
}

PosList::~PosList()
{
DOTRACE("PosList::~PosList");
}

PosList& PosList::thePosList() {
DOTRACE("PosList::thePosList");
  return theInstance;
}

///////////////////////////////////////////////////////////////////////
//
// Template instantiation of PtrList<Position>
//
///////////////////////////////////////////////////////////////////////

#include "position.h"
#include "ptrlist.cc"
template class PtrList<Position>;

template <>
PtrList<Position>& ItemWithId<Position>::ptrList()
{ return PosList::thePosList(); }

template <>
PtrList<Position>& NullableItemWithId<Position>::ptrList()
{ return PosList::thePosList(); }

#include "itemwithid.cc"
template class ItemWithId<Position>;
template class NullableItemWithId<Position>;

static const char vcid_poslist_cc[] = "$Header$";
#endif // !POSLIST_CC_DEFINED
