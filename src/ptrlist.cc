///////////////////////////////////////////////////////////////////////
//
// ptrlist.cc
// Rob Peters
// created: Fri Apr 23 00:35:32 1999
// written: Wed Oct 25 09:32:46 2000
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
#else
#error not supported
#endif
}

template <class T>
int PtrList<T>::count() const
{
#ifdef OLD_VERSION
  return PtrListBase::count();
#else
#error not supported
#endif
}

template <class T>
bool PtrList<T>::isValidId(int id) const
{
#ifdef OLD_VERSION
  return PtrListBase::isValidId(id);
#else
#error not supported
#endif
}

template <class T>
void PtrList<T>::remove(int id)
{
#ifdef OLD_VERSION
  PtrListBase::remove(id);
#else
#error not supported
#endif
}

template <class T>
void PtrList<T>::release(int id)
{
#ifdef OLD_VERSION
  PtrListBase::release(id);
#else
#error not supported
#endif
}

template <class T>
void PtrList<T>::clear()
{
#ifdef OLD_VERSION
  PtrListBase::clear();
#else
#error not supported
#endif
}

template <class T>
PtrList<T>::SharedPtr PtrList<T>::getCheckedPtr(int id) const
{
#ifdef OLD_VERSION
  IO::IoObject* voidPtr = PtrListBase::getCheckedPtrBase(id);
  // cast as reference to force an exception on error
  T& t = dynamic_cast<T&>(*voidPtr);
  return SharedPtr(&t, id);
#else
#error not supported
#endif
}

template <class T>
PtrList<T>::SharedPtr PtrList<T>::insert(T* master)
{
#ifdef OLD_VERSION
  return SharedPtr(master, PtrListBase::insertPtrBase(master));
#else
#error not supported
#endif
}

template <class T>
IO::IoObject* PtrList<T>::getCheckedPtrBase(int id) const throw (InvalidIdError)
{
#ifdef OLD_VERSION
  return PtrListBase::getCheckedPtrBase(id);
#else
#error not supported
#endif
}

template <class T>
int PtrList<T>::insertPtrBase(IO::IoObject* ptr)
{
#ifdef OLD_VERSION
  return PtrListBase::insertPtrBase(ptr);
#else
#error not supported
#endif
}

template <class T>
void PtrList<T>::ensureCorrectType(const IO::IoObject* ptr) const
{
  const T* t = dynamic_cast<const T*>(ptr);
  if (t == 0)
	 throw ErrorWithMsg("ensureCorrectType failed");
}

static const char vcid_ptrlist_cc[] = "$Header$";
#endif // !PTRLIST_CC_DEFINED
