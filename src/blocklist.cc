///////////////////////////////////////////////////////////////////////
//
// blocklist.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Jun  9 20:22:18 1999
// written: Thu Oct 26 17:52:56 2000
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

BlockList::BlockList() : Base() {
DOTRACE("BlockList::BlockList");
  // nothing
}

BlockList::~BlockList() {}

BlockList BlockList::theInstance;

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
template class PtrList<Block>;

template <>
PtrList<Block>& IdItem<Block>::ptrList()
{ return BlockList::theBlockList(); }

template <>
PtrList<Block>& MaybeIdItem<Block>::ptrList()
{ return BlockList::theBlockList(); }

#include "iditem.cc"
template class IdItem<Block>;
template class MaybeIdItem<Block>;

static const char vcid_blocklist_cc[] = "$Header$";
#endif // !BLOCKLIST_CC_DEFINED
