///////////////////////////////////////////////////////////////////////
//
// ptrlist.cc
// Rob Peters
// created: Fri Apr 23 00:35:32 1999
// written: Sun Oct 22 15:15:28 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef PTRLIST_CC_DEFINED
#define PTRLIST_CC_DEFINED

#include "ptrlist.h"

template <class T>
PtrList<T>::PtrList(int size) :
  IoPtrList(size)
{}

template <class T>
PtrList<T>::~PtrList() {}

template <class T>
PtrList<T>::SharedPtr PtrList<T>::getPtr(int id) const
    {
		RefCounted* voidPtr = PtrListBase::getPtrBase(id);
		// cast as reference to force an exception on error
		T& t = dynamic_cast<T&>(*voidPtr);
		return SharedPtr(&t, id);
	 }

template <class T>
PtrList<T>::SharedPtr PtrList<T>::getCheckedPtr(int id) const
	 {
		RefCounted* voidPtr = PtrListBase::getCheckedPtrBase(id);
		// cast as reference to force an exception on error
		T& t = dynamic_cast<T&>(*voidPtr);
		return SharedPtr(&t, id);
	 }

template <class T>
PtrList<T>::SharedPtr PtrList<T>::insert(T* master)
	 { return SharedPtr(master, PtrListBase::insertPtrBase(master)); }

template <class T>
void PtrList<T>::insertAt(int id, T* master)
	 { PtrListBase::insertPtrBaseAt(id, master); }

static const char vcid_ptrlist_cc[] = "$Header$";
#endif // !PTRLIST_CC_DEFINED
