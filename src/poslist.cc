///////////////////////////////////////////////////////////////////////
//
// poslist.cc
// Rob Peters
// created: Fri Mar 12 17:21:29 1999
// written: Tue Oct 17 12:11:25 2000
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

namespace {
  const int DEFAULT_INIT_SIZE = 10;
}

PosList PosList::theInstance(DEFAULT_INIT_SIZE);

PosList::PosList(int size) : Base(size)
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
#include "masterptrt.cc"
template class MasterPtr<Position>;
template class PtrList<Position>;

template <>
PtrList<Position>& ItemWithId<Position>::ptrList()
{ return PosList::thePosList(); }

template <>
PtrList<Position>& NullableItemWithId<Position>::ptrList()
{ return PosList::thePosList(); }

static const char vcid_poslist_cc[] = "$Header$";
#endif // !POSLIST_CC_DEFINED
