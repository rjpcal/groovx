///////////////////////////////////////////////////////////////////////
//
// rhlist.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Jun  9 20:05:29 1999
// written: Mon Oct 23 11:45:22 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef RHLIST_CC_DEFINED
#define RHLIST_CC_DEFINED

#include "rhlist.h"

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// Template instantiation of PtrList<ResponseHandler>
//
///////////////////////////////////////////////////////////////////////

#include "responsehandler.h"
#include "ptrlist.cc"
template class PtrList<ResponseHandler>;
#include "itemwithid.cc"
template class ItemWithId<ResponseHandler>;
template class NullableItemWithId<ResponseHandler>;

template <>
PtrList<ResponseHandler>& ItemWithId<ResponseHandler>::ptrList()
{ return RhList::theRhList(); }

template <>
PtrList<ResponseHandler>& NullableItemWithId<ResponseHandler>::ptrList()
{ return RhList::theRhList(); }

///////////////////////////////////////////////////////////////////////
//
// File scope declarations
//
///////////////////////////////////////////////////////////////////////

namespace {
  const int DEFAULT_INIT_SIZE = 10;
}

///////////////////////////////////////////////////////////////////////
//
// RhList member definitions
//
///////////////////////////////////////////////////////////////////////

RhList::RhList(int size) :
  Base(size)
{
DOTRACE("RhList::RhList");
}

RhList::~RhList() {}

RhList RhList::theInstance(DEFAULT_INIT_SIZE);

RhList& RhList::theRhList() {
DOTRACE("RhList::theRhList");
  return theInstance; 
}

static const char vcid_rhlist_cc[] = "$Header$";
#endif // !RHLIST_CC_DEFINED
