///////////////////////////////////////////////////////////////////////
//
// thlist.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Jun  9 20:05:37 1999
// written: Fri Oct 27 17:49:19 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef THLIST_CC_DEFINED
#define THLIST_CC_DEFINED

#include "thlist.h"

#include "ioptrlist.h"

#define NO_TRACE
#include "util/trace.h"

IoPtrList& ThList::theThList() {
DOTRACE("ThList::theThList");
  return IoPtrList::theList();
}

///////////////////////////////////////////////////////////////////////
//
// Template instantiation of PtrList<TimingHdlr>
//
///////////////////////////////////////////////////////////////////////

#include "timinghdlr.h"

template <>
IoPtrList& IdItem<TimingHdlr>::ptrList()
{ return ThList::theThList(); }

template <>
IoPtrList& MaybeIdItem<TimingHdlr>::ptrList()
{ return ThList::theThList(); }

static const char vcid_thlist_cc[] = "$Header$";
#endif // !THLIST_CC_DEFINED
