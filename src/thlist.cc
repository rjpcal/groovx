///////////////////////////////////////////////////////////////////////
//
// thlist.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Jun  9 20:05:37 1999
// written: Thu Oct 26 17:52:56 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef THLIST_CC_DEFINED
#define THLIST_CC_DEFINED

#include "thlist.h"

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

ThList::ThList() : Base()
{
DOTRACE("ThList::ThList");
}

ThList::~ThList() {}

ThList ThList::theInstance;

ThList& ThList::theThList() {
DOTRACE("ThList::theThList");
  return theInstance; 
}

///////////////////////////////////////////////////////////////////////
//
// Template instantiation of PtrList<TimingHdlr>
//
///////////////////////////////////////////////////////////////////////

#include "timinghdlr.h"
#include "ptrlist.cc"
template class PtrList<TimingHdlr>;

template <>
PtrList<TimingHdlr>& IdItem<TimingHdlr>::ptrList()
{ return ThList::theThList(); }

template <>
PtrList<TimingHdlr>& MaybeIdItem<TimingHdlr>::ptrList()
{ return ThList::theThList(); }

#include "iditem.cc"
template class IdItem<TimingHdlr>;
template class MaybeIdItem<TimingHdlr>;

static const char vcid_thlist_cc[] = "$Header$";
#endif // !THLIST_CC_DEFINED
