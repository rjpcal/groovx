///////////////////////////////////////////////////////////////////////
//
// ptrlist.cc
// Rob Peters
// created: Fri Apr 23 00:35:32 1999
// written: Thu Oct 26 10:45:03 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef PTRLIST_CC_DEFINED
#define PTRLIST_CC_DEFINED

#include "ptrlist.h"

#define OLD_VERSION 1

template <class T>
PtrList<T>::PtrList() :
  IoPtrList()
{}

template <class T>
PtrList<T>::~PtrList() {}

template <class T>
PtrList<T>::SharedPtr PtrList<T>::insert(T* master)
{
  return SharedPtr(master, insertPtrBase(master));
}

template <class T>
PtrList<T>::SharedPtr PtrList<T>::getCheckedPtr(int id) const
{
  IO::IoObject* voidPtr = getCheckedPtrBase(id);
  // cast as reference to force an exception on error
  T& t = dynamic_cast<T&>(*voidPtr);
  return SharedPtr(&t, id);
}

template <class T>
void PtrList<T>::ensureCorrectType(const IO::IoObject* ptr) const
{
  const T& t = dynamic_cast<const T&>(*ptr);
}

static const char vcid_ptrlist_cc[] = "$Header$";
#endif // !PTRLIST_CC_DEFINED
