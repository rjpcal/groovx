///////////////////////////////////////////////////////////////////////
//
// thlist.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Jun  9 20:05:37 1999
// written: Mon Oct  9 08:34:53 2000
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

namespace {
  const int DEFAULT_INIT_SIZE = 10;
}

ThList::ThList(int size) : Base(size)
{
DOTRACE("ThList::ThList");
  //
}

ThList::~ThList() {}

ThList ThList::theInstance(DEFAULT_INIT_SIZE);

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
#include "masterptrt.cc"
template class MasterPtr<TimingHdlr>;
template class PtrList<TimingHdlr>;

static const char vcid_thlist_cc[] = "$Header$";
#endif // !THLIST_CC_DEFINED
