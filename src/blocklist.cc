///////////////////////////////////////////////////////////////////////
//
// blocklist.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Jun  9 20:22:18 1999
// written: Tue Oct 17 12:09:55 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BLOCKLIST_CC_DEFINED
#define BLOCKLIST_CC_DEFINED

#include "blocklist.h"

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

namespace {
  const int DEFAULT_INIT_SIZE = 10;
}

BlockList::BlockList(int size) : Base(size) {
DOTRACE("BlockList::BlockList");
  // nothing
}

BlockList::~BlockList() {}

BlockList BlockList::theInstance(DEFAULT_INIT_SIZE);

BlockList& BlockList::theBlockList() {
DOTRACE("BlockList::theBlockList");
  return theInstance;
}

///////////////////////////////////////////////////////////////////////
//
// Template instantiation for PtrList<Block>
//
///////////////////////////////////////////////////////////////////////

#include "block.h"
#include "ptrlist.cc"
#include "masterptrt.cc"
template class PtrList<Block>;
template class MasterPtr<Block>;

template <>
PtrList<Block>& ItemWithId<Block>::ptrList()
{ return BlockList::theBlockList(); }

template <>
PtrList<Block>& NullableItemWithId<Block>::ptrList()
{ return BlockList::theBlockList(); }

static const char vcid_blocklist_cc[] = "$Header$";
#endif // !BLOCKLIST_CC_DEFINED
