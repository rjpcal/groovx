///////////////////////////////////////////////////////////////////////
//
// blocklist.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Jun  9 20:22:18 1999
// written: Fri Oct 27 17:49:10 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BLOCKLIST_CC_DEFINED
#define BLOCKLIST_CC_DEFINED

#include "blocklist.h"

#include "ioptrlist.h"

#define NO_TRACE
#include "util/trace.h"

IoPtrList& BlockList::theBlockList() {
DOTRACE("BlockList::theBlockList");
  return IoPtrList::theList(); 
}

///////////////////////////////////////////////////////////////////////
//
// Template instantiation for PtrList<Block>
//
///////////////////////////////////////////////////////////////////////

#include "block.h"

template <>
IoPtrList& IdItem<Block>::ptrList()
{ return BlockList::theBlockList(); }

template <>
IoPtrList& MaybeIdItem<Block>::ptrList()
{ return BlockList::theBlockList(); }

static const char vcid_blocklist_cc[] = "$Header$";
#endif // !BLOCKLIST_CC_DEFINED
