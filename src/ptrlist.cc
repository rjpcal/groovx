///////////////////////////////////////////////////////////////////////
//
// ptrlist.cc
// Rob Peters
// created: Fri Apr 23 00:35:32 1999
// written: Thu Oct 19 17:36:41 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef PTRLIST_CC_DEFINED
#define PTRLIST_CC_DEFINED

#include "ptrlist.h"

#include "system/demangle.h"

#include "util/strings.h"

#include <typeinfo>

template <class T>
PtrList<T>::PtrList(int size) :
  IoPtrList(size)
{}

template <class T>
PtrList<T>::~PtrList() {}

template <class T>
MasterIoPtr* PtrList<T>::makeMasterIoPtr(IO::IoObject* obj) const {
  T& t = dynamic_cast<T&>(*obj);

  return new MasterPtr<T>(&t);
}

static const char vcid_ptrlist_cc[] = "$Header$";
#endif // !PTRLIST_CC_DEFINED
