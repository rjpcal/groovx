///////////////////////////////////////////////////////////////////////
//
// objlist.h 
// Rob Peters
// Created: Nov-98
// written: Thu Jul 22 12:55:03 1999
// $Id$
//
//
// This file defines ObjList, a singleton class whose instance is a
// global repository of GrObj pointers. It allows GrObj's to be
// uniquely identified by an objid from anywhere in the
// application. In addition, the entire collection can be written to
// or read from a stream. The singleton instance is retrieved with the
// function theObjList().
//
///////////////////////////////////////////////////////////////////////

#ifndef OBJLIST_H_DEFINED
#define OBJLIST_H_DEFINED

#ifndef PTRLIST_H_DEFINED
#include "ptrlist.h"
#endif

#include "id.h"

class GrObj;

///////////////////////////////////////////////////////////////////////
//
// ObjList class definition
//
///////////////////////////////////////////////////////////////////////

class ObjList : public PtrList<GrObj>, public virtual IO {
private:
  typedef PtrList<GrObj> Base;

protected:
  // Construct a list of 'size' GrObj*'s, initialized to NULL
  ObjList (int size) : Base(size) {}

public:
  // Returns a reference to the singleton instance of ObjList
  static ObjList& theObjList();
};

static const char vcid_objlist_h[] = "$Header$";
#endif // !OBJLIST_H_DEFINED
