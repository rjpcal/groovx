///////////////////////////////////////////////////////////////////////
//
// blocklist.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Jun  9 20:22:18 1999
// written: Sat Jun 26 12:32:00 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BLOCKLIST_CC_DEFINED
#define BLOCKLIST_CC_DEFINED

#include "blocklist.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

namespace {
  BlockList* instance = 0;
  const int DEFAULT_INIT_SIZE = 10;
}

BlockList::BlockList(int size) : Base(size) {
DOTRACE("BlockList::BlockList");
  // nothing
}

BlockList& BlockList::theBlockList() {
DOTRACE("BlockList::theBlockList");
  if (instance == 0) {
	 instance = new BlockList(DEFAULT_INIT_SIZE);
  }
  return *instance;
}

BlockList::~BlockList() {
DOTRACE("BlockList::~BlockList");
  // nothing
}

///////////////////////////////////////////////////////////////////////
//
// Template instantiation for PtrList<Expt>
//
///////////////////////////////////////////////////////////////////////

#include "block.h"
#include "ptrlist.cc"
template class PtrList<Expt>;

static const char vcid_blocklist_cc[] = "$Header$";
#endif // !BLOCKLIST_CC_DEFINED
