///////////////////////////////////////////////////////////////////////
//
// iditem.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Oct 26 17:51:16 2000
// written: Fri Oct 27 15:50:12 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IDITEM_CC_DEFINED
#define IDITEM_CC_DEFINED

#include "iditem.h"

#include "util/error.h"

///////////////////////////////////////////////////////////////////////
//
// IdItemUtils
//
///////////////////////////////////////////////////////////////////////

#if 0
bool IdItemUtils::isValidId(int id) {
  return IoPtrList::theList().isValidId(id);
}

void IdItemUtils::insertItem(IO::IoObject* obj) {
  IoPtrList::theList().insertPtrBase(obj);
}

IO::IoObject* IdItemUtils::getCheckedItem(int id) {
  return IoPtrList::theList().getCheckedPtrBase(int id);
}
#endif

///////////////////////////////////////////////////////////////////////
//
// IdItem member definitions
//
///////////////////////////////////////////////////////////////////////

template <class T>
IdItem<T>::IdItem(int id) :
  itsHandle(ptrList().template getCheckedIoPtr<T>(id).handle())
{}

template <class T>
IdItem<T>::IdItem(T* ptr, Insert /*dummy param*/) :
  itsHandle(ptr)
{
  ptrList().template insertIo<T>(ptr);
}

template <class T>
IdItem<T>::IdItem(PtrHandle<T> item, Insert /*dummy param*/) :
  itsHandle(item)
{
  ptrList().template insertIo<T>(itsHandle.get());
}

template <class T>
IdItem<T>& IdItem<T>::operator=(T* new_master)
{
  itsHandle = PtrHandle<T>(new_master);
  ptrList().template insertIo<T>(new_master);
  return *this;
}

///////////////////////////////////////////////////////////////////////
//
// MaybeIdItem member definitions
//
///////////////////////////////////////////////////////////////////////

template <class T>
void MaybeIdItem<T>::check() {
}

template <class T>
MaybeIdItem<T>::MaybeIdItem(T* master) :
  itsHandle(master),
  itsId(-1)
{
  if (master != 0)
	 {
		ptrList().template insertIo<T>(master);
		itsId = master->id();
	 }
}

template <class T>
MaybeIdItem<T>::MaybeIdItem(PtrHandle<T> item) :
  itsHandle(item),
  itsId(item->id())
{
  ptrList().template insertIo<T>(itsHandle.get());
}

template <class T>
void MaybeIdItem<T>::refresh() const {
  if ( !itsHandle.isValid() )
	 {
		IdItem<T> p = ptrList().template getCheckedIoPtr<T>(itsId);
		itsHandle = p.handle();
		Assert(itsId == itsHandle->id());
	 }
}

template <class T>
void MaybeIdItem<T>::attemptRefresh() const {
  if ( !itsHandle.isValid() )
	 {
		if (ptrList().isValidId(itsId)) {
		  IdItem<T> p = ptrList().template getCheckedIoPtr<T>(itsId);
		  itsHandle = p.handle();
		  Assert(itsId == itsHandle->id());
		}
	 }
}

static const char vcid_iditem_cc[] = "$Header$";
#endif // !IDITEM_CC_DEFINED
