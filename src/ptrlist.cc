///////////////////////////////////////////////////////////////////////
//
// ptrlist.cc
// Rob Peters
// created: Fri Apr 23 00:35:32 1999
// written: Fri Oct 27 14:12:25 2000
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
  int id = insertPtrBase(master);

  Assert( id == master->id() );

  return SharedPtr(master);
}

template <class T>
PtrList<T>::SharedPtr PtrList<T>::getCheckedPtr(int id) const
{
  IO::IoObject* voidPtr = getCheckedPtrBase(id);

  // cast as reference to force an exception on error
  T& t = dynamic_cast<T&>(*voidPtr);

  Assert( t.id() == id );

  return SharedPtr(&t);
}

template <class T>
void PtrList<T>::ensureCorrectType(const IO::IoObject* ptr) const
{
  const T& t = dynamic_cast<const T&>(*ptr);
}

static const char vcid_ptrlist_cc[] = "$Header$";
#endif // !PTRLIST_CC_DEFINED
