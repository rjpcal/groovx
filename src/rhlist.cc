///////////////////////////////////////////////////////////////////////
//
// rhlist.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Jun  9 20:05:29 1999
// written: Wed Oct 11 13:31:20 2000
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
#include "masterptrt.cc"
template class MasterPtr<ResponseHandler>;
template class PtrList<ResponseHandler>;

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
  Base(size),
  itsInterp(0)
{
DOTRACE("RhList::RhList");
}

RhList::~RhList() {}

RhList RhList::theInstance(DEFAULT_INIT_SIZE);

RhList& RhList::theRhList() {
DOTRACE("RhList::theRhList");
  return theInstance; 
}

void RhList::afterInsertHook(int id, MasterPtrBase* /* ptr */) {
DOTRACE("RhList::afterInsertHook");
  SharedPtr rh = getCheckedPtr(id);
  rh->setInterp(itsInterp);
}

void RhList::setInterp(Tcl_Interp* interp) {
DOTRACE("RhList::setInterp");
  itsInterp = interp;
  for (int i = 0; i < capacity(); ++i) {
	 if (isValidId(i)) {
		getPtr(i)->setInterp(itsInterp);
	 }
  }
}

static const char vcid_rhlist_cc[] = "$Header$";
#endif // !RHLIST_CC_DEFINED
