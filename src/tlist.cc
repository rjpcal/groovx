///////////////////////////////////////////////////////////////////////
//
// tlist.cc
// Rob Peters
// created: Fri Mar 12 14:39:39 1999
// written: Wed Oct 25 17:52:51 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TLIST_CC_DEFINED
#define TLIST_CC_DEFINED

#include "tlist.h"

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// Tlist member definitions
//
///////////////////////////////////////////////////////////////////////

Tlist::Tlist() : PtrList<TrialBase>()
{
DOTRACE("Tlist::Tlist");
}

Tlist::~Tlist() {}

Tlist Tlist::theInstance;

Tlist& Tlist::theTlist() {
DOTRACE("Tlist::theTlist");
  return theInstance;
}

///////////////////////////////////////////////////////////////////////
//
// Template instantiation of PtrList<TrialBase>
//
///////////////////////////////////////////////////////////////////////

#include "trialbase.h"
#include "ptrlist.cc"
template class PtrList<TrialBase>;

template <>
PtrList<TrialBase>& ItemWithId<TrialBase>::ptrList()
{ return Tlist::theTlist(); }

template <>
PtrList<TrialBase>& NullableItemWithId<TrialBase>::ptrList()
{ return Tlist::theTlist(); }

#include "itemwithid.cc"
template class ItemWithId<TrialBase>;
template class NullableItemWithId<TrialBase>;

static const char vcid_tlist_cc[] = "$Header$";
#endif // !TLIST_CC_DEFINED
