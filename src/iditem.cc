///////////////////////////////////////////////////////////////////////
//
// iditem.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Oct 26 17:51:16 2000
// written: Fri Oct 27 13:14:28 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IDITEM_CC_DEFINED
#define IDITEM_CC_DEFINED

#include "iditem.h"

#include "util/error.h"

///////////////////////////////////////////////////////////////////////
//
// IdItem member definitions
//
///////////////////////////////////////////////////////////////////////

template <class T>
IdItem<T>::IdItem(int id) :
  itsHandle(ptrList().getCheckedPtr(id).handle())
{}

template <class T>
IdItem<T>::IdItem(T* ptr, Insert /*dummy param*/) :
  itsHandle(ptr)
{
  ptrList().insert(itsHandle);
}

template <class T>
IdItem<T>::IdItem(PtrHandle<T> item, Insert /*dummy param*/) :
  itsHandle(item)
{
  ptrList().insert(itsHandle);
}

template <class T>
IdItem<T>& IdItem<T>::operator=(T* new_master)
{
  itsHandle = PtrHandle<T>(new_master);
  ptrList().insert(itsHandle);
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
		itsId = ptrList().insert(master).id();
	 }
}

template <class T>
MaybeIdItem<T>::MaybeIdItem(PtrHandle<T> item) :
  itsHandle(item),
  itsId(ptrList().insert(itsHandle.get()).id())
{}

template <class T>
void MaybeIdItem<T>::refresh() const {
  if ( !itsHandle.isValid() )
	 {
		typename PtrList<T>::SharedPtr p = ptrList().getCheckedPtr(itsId);
		itsHandle = p.handle();
		Assert(itsId == itsHandle->id());
	 }
}

template <class T>
void MaybeIdItem<T>::attemptRefresh() const {
  if ( !itsHandle.isValid() )
	 {
		if (ptrList().isValidId(itsId)) {
		  typename PtrList<T>::SharedPtr p = ptrList().getCheckedPtr(itsId);
		  itsHandle = p.handle();
		  Assert(itsId == itsHandle->id());
		}
	 }
}

static const char vcid_iditem_cc[] = "$Header$";
#endif // !IDITEM_CC_DEFINED
