///////////////////////////////////////////////////////////////////////
//
// rhlist.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Jun  9 20:05:29 1999
// written: Thu Oct 26 17:52:56 2000
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
PtrList<ResponseHandler>& IdItem<ResponseHandler>::ptrList()
{ return RhList::theRhList(); }

template <>
PtrList<ResponseHandler>& MaybeIdItem<ResponseHandler>::ptrList()
{ return RhList::theRhList(); }

#include "iditem.cc"
template class IdItem<ResponseHandler>;
template class MaybeIdItem<ResponseHandler>;

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
