///////////////////////////////////////////////////////////////////////
//
// rhlist.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Jun  9 20:05:29 1999
// written: Fri Oct 27 17:49:14 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef RHLIST_CC_DEFINED
#define RHLIST_CC_DEFINED

#include "rhlist.h"

#include "ioptrlist.h"

#define NO_TRACE
#include "util/trace.h"

IoPtrList& RhList::theRhList() {
DOTRACE("RhList::theRhList");
  return IoPtrList::theList(); 
}

///////////////////////////////////////////////////////////////////////
//
// Template instantiation of PtrList<ResponseHandler>
//
///////////////////////////////////////////////////////////////////////

#include "responsehandler.h"

template <>
IoPtrList& IdItem<ResponseHandler>::ptrList()
{ return RhList::theRhList(); }

template <>
IoPtrList& MaybeIdItem<ResponseHandler>::ptrList()
{ return RhList::theRhList(); }

static const char vcid_rhlist_cc[] = "$Header$";
#endif // !RHLIST_CC_DEFINED
