///////////////////////////////////////////////////////////////////////
//
// objlist.cc
// Rob Peters
// created: Nov-98
// written: Mon May 15 19:34:09 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef OBJLIST_CC_DEFINED
#define OBJLIST_CC_DEFINED

#include "objlist.h"

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

namespace {
  const int DEFAULT_INIT_SIZE = 10;
}

ObjList ObjList::theInstance(DEFAULT_INIT_SIZE);

ObjList& ObjList::theObjList() {
DOTRACE("ObjList::theObjList");
  return theInstance; 
}

///////////////////////////////////////////////////////////////////////
//
// Template instantiation of PtrList<GrObj>
//
///////////////////////////////////////////////////////////////////////

#include "grobj.h"
#include "ptrlist.cc"
template class PtrList<GrObj>;

static const char vcid_objlist_cc[] = "$Header$";
#endif // !OBJLIST_CC_DEFINED
