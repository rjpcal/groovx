///////////////////////////////////////////////////////////////////////
//
// tlist.cc
// Rob Peters
// created: Fri Mar 12 14:39:39 1999
// written: Fri Oct 27 18:12:04 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TLIST_CC_DEFINED
#define TLIST_CC_DEFINED

#include "tlist.h"

#include "ioptrlist.h"

#define NO_TRACE
#include "util/trace.h"

IoPtrList& Tlist::theTlist() {
DOTRACE("Tlist::theTlist");
  return IoPtrList::theList();
}

///////////////////////////////////////////////////////////////////////
//
// Template instantiation of PtrList<TrialBase>
//
///////////////////////////////////////////////////////////////////////

#include "trialbase.h"

template <>
IoPtrList& IdItem<TrialBase>::ptrList()
{ return Tlist::theTlist(); }

template <>
IoPtrList& MaybeIdItem<TrialBase>::ptrList()
{ return Tlist::theTlist(); }

static const char vcid_tlist_cc[] = "$Header$";
#endif // !TLIST_CC_DEFINED
