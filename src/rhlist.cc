///////////////////////////////////////////////////////////////////////
//
// rhlist.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Jun  9 20:05:29 1999
// written: Wed Oct 25 17:52:22 2000
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

template <>
PtrList<ResponseHandler>& ItemWithId<ResponseHandler>::ptrList()
{ return RhList::theRhList(); }

template <>
PtrList<ResponseHandler>& NullableItemWithId<ResponseHandler>::ptrList()
{ return RhList::theRhList(); }

#include "itemwithid.cc"
template class ItemWithId<ResponseHandler>;
template class NullableItemWithId<ResponseHandler>;

///////////////////////////////////////////////////////////////////////
//
// RhList member definitions
//
///////////////////////////////////////////////////////////////////////

RhList::RhList() :
  Base()
{
DOTRACE("RhList::RhList");
}

RhList::~RhList() {}

RhList RhList::theInstance;

RhList& RhList::theRhList() {
DOTRACE("RhList::theRhList");
  return theInstance; 
}

static const char vcid_rhlist_cc[] = "$Header$";
#endif // !RHLIST_CC_DEFINED
