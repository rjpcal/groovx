///////////////////////////////////////////////////////////////////////
//
// objlist.h 
// Rob Peters
// Created: Nov-98
// written: Mon May 15 19:33:50 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef OBJLIST_H_DEFINED
#define OBJLIST_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(PTRLIST_H_DEFINED)
#include "ptrlist.h"
#endif

class GrObj;

///////////////////////////////////////////////////////////////////////
/**
 *
 * ObjList is a singleton wrapper for \c PtrList<GrObj>.
 *
 **/
///////////////////////////////////////////////////////////////////////

class ObjList : public PtrList<GrObj> {
private:
  typedef PtrList<GrObj> Base;

  static ObjList theInstance;

protected:
  /// Construct and reserve space for \a size objects.
  ObjList (int size) : Base(size) {}

public:
  /// Returns a reference to the singleton instance.
  static ObjList& theObjList();
};

static const char vcid_objlist_h[] = "$Header$";
#endif // !OBJLIST_H_DEFINED
