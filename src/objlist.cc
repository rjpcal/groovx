///////////////////////////////////////////////////////////////////////
//
// objlist.cc
// Rob Peters
// created: Nov-98
// written: Fri Oct 27 18:12:50 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef OBJLIST_CC_DEFINED
#define OBJLIST_CC_DEFINED

#include "objlist.h"

#include "ioptrlist.h"

#define NO_TRACE
#include "util/trace.h"

IoPtrList& ObjList::theObjList() {
DOTRACE("ObjList::theObjList");
  return IoPtrList::theList(); 
}

///////////////////////////////////////////////////////////////////////
//
// Template instantiation of PtrList<GrObj>
//
///////////////////////////////////////////////////////////////////////

#include "grobj.h"

template <>
IoPtrList& IdItem<GrObj>::ptrList()
{ return ObjList::theObjList(); }

template <>
IoPtrList& MaybeIdItem<GrObj>::ptrList()
{ return ObjList::theObjList(); }

static const char vcid_objlist_cc[] = "$Header$";
#endif // !OBJLIST_CC_DEFINED
