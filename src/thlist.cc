///////////////////////////////////////////////////////////////////////
//
// thlist.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Jun  9 20:05:37 1999
// written: Wed Feb 16 07:50:45 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef THLIST_CC_DEFINED
#define THLIST_CC_DEFINED

#include "thlist.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

namespace {
  ThList* instance = 0;
  const int DEFAULT_INIT_SIZE = 10;
}

ThList::ThList(int size) : Base(size)
{
DOTRACE("ThList::ThList ");
  //
}

ThList& ThList::theThList() {
DOTRACE("ThList::theThList");
  if (instance == 0) {
	 instance = new ThList(DEFAULT_INIT_SIZE);
  }
  return *instance;
}

///////////////////////////////////////////////////////////////////////
//
// Template instantiation of PtrList<TimingHdlr>
//
///////////////////////////////////////////////////////////////////////

#include "timinghdlr.h"
#include "ptrlist.cc"
template class PtrList<TimingHdlr>;

static const char vcid_thlist_cc[] = "$Header$";
#endif // !THLIST_CC_DEFINED
