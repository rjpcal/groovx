///////////////////////////////////////////////////////////////////////
//
// poslist.cc
// Rob Peters
// created: Fri Mar 12 17:21:29 1999
// written: Wed Mar 15 10:17:27 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef POSLIST_CC_DEFINED
#define POSLIST_CC_DEFINED

#include "poslist.h"

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

namespace {
  PosList* instance = 0;
  const int DEFAULT_INIT_SIZE = 10;
}

PosList& PosList::thePosList() {
DOTRACE("PosList::thePosList");
  if (instance == 0) {
	 instance = new PosList(DEFAULT_INIT_SIZE);
  }
  return *instance;
}

///////////////////////////////////////////////////////////////////////
//
// Template instantiation of PtrList<Position>
//
///////////////////////////////////////////////////////////////////////

#include "position.h"
#include "ptrlist.cc"
template class PtrList<Position>;

static const char vcid_poslist_cc[] = "$Header$";
#endif // !POSLIST_CC_DEFINED
