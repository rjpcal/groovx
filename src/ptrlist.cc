///////////////////////////////////////////////////////////////////////
//
// ptrlist.cc
// Rob Peters
// created: Fri Apr 23 00:35:32 1999
// written: Tue Oct 24 17:20:45 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef PTRLIST_CC_DEFINED
#define PTRLIST_CC_DEFINED

#include "ptrlist.h"

#define OLD_VERSION 1

template <class T>
PtrList<T>::PtrList(int size) :
  IoPtrList(size)
{}

template <class T>
PtrList<T>::~PtrList() {}

template <class T>
int PtrList<T>::capacity() const
{
#ifdef OLD_VERSION
  return PtrListBase::capacity();
#endif
}

template <class T>
int PtrList<T>::count() const
{
#ifdef OLD_VERSION
  return PtrListBase::count();
#endif
}

template <class T>
bool PtrList<T>::isValidId(int id) const
{
#ifdef OLD_VERSION
  return PtrListBase::isValidId(id);
#endif
}

template <class T>
void PtrList<T>::remove(int id)
{
#ifdef OLD_VERSION
  PtrListBase::remove(id);
#endif
}

template <class T>
void PtrList<T>::release(int id)
{
#ifdef OLD_VERSION
  PtrListBase::release(id);
#endif
}

template <class T>
void PtrList<T>::clear()
{
#ifdef OLD_VERSION
  PtrListBase::clear();
#endif
}

template <class T>
PtrList<T>::SharedPtr PtrList<T>::getCheckedPtr(int id) const
{
#ifdef OLD_VERSION
  RefCounted* voidPtr = PtrListBase::getCheckedPtrBase(id);
  // cast as reference to force an exception on error
  T& t = dynamic_cast<T&>(*voidPtr);
  return SharedPtr(&t, id);
#endif
}

template <class T>
PtrList<T>::SharedPtr PtrList<T>::insert(T* master)
{
#ifdef OLD_VERSION
  return SharedPtr(master, PtrListBase::insertPtrBase(master));
#endif
}

static const char vcid_ptrlist_cc[] = "$Header$";
#endif // !PTRLIST_CC_DEFINED
