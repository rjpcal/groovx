///////////////////////////////////////////////////////////////////////
//
// rhlist.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Jun  9 20:05:29 1999
// written: Thu Nov 11 15:01:49 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef RHLIST_CC_DEFINED
#define RHLIST_CC_DEFINED

#include "rhlist.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

///////////////////////////////////////////////////////////////////////
//
// Template instantiation of PtrList<ResponseHandler>
//
///////////////////////////////////////////////////////////////////////

#include "responsehandler.h"
#include "ptrlist.cc"
template class PtrList<ResponseHandler>;

///////////////////////////////////////////////////////////////////////
//
// File scope declarations
//
///////////////////////////////////////////////////////////////////////

namespace {
  RhList* instance = 0;
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

RhList& RhList::theRhList() {
DOTRACE("RhList::theRhList");
  if (instance == 0) {
	 instance = new RhList(DEFAULT_INIT_SIZE);
  }
  return *instance;
}

void RhList::deserialize(istream &is, IOFlag flag) {
DOTRACE("RhList::deserialize");
  Base::deserialize(is, flag);
  
  // This ensures that all the new ResponseHandler's have their
  // Tcl_Interp* set correctly
  setInterp(itsInterp);
}

void RhList::readFrom(Reader* reader) {
DOTRACE("RhList::readFrom");

  Base::readFrom(reader);

  // Ensure that all the new ResponseHandler's have their Tcl_Interp*
  // set correctly
  setInterp(itsInterp);
}

int RhList::insert(ResponseHandler* ptr) {
DOTRACE("RhList::insert");
  ptr->setInterp(itsInterp);
  return Base::insert(ptr);
}

void RhList::insertAt(int id, ResponseHandler* ptr) {
DOTRACE("RhList::insertAt");
  ptr->setInterp(itsInterp);
  Base::insertAt(id, ptr);
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
