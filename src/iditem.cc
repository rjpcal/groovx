///////////////////////////////////////////////////////////////////////
//
// iditem.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Oct 26 17:51:16 2000
// written: Thu Oct 26 17:51:33 2000
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
  itsHandle(ptrList().getCheckedPtr(id).handle()),
  itsId(id)
{}

template <class T>
IdItem<T>::IdItem(T* ptr, Insert /*dummy param*/) :
  itsHandle(ptr),
  itsId(ptrList().insert(itsHandle).id())
{}

template <class T>
IdItem<T>::IdItem(PtrHandle<T> item, Insert /*dummy param*/) :
  itsHandle(item),
  itsId(ptrList().insert(itsHandle).id())
{}

template <class T>
IdItem<T>& IdItem<T>::operator=(T* new_master)
{
  itsHandle = PtrHandle<T>(new_master);
  itsId = ptrList().insert(itsHandle).id();
}

///////////////////////////////////////////////////////////////////////
//
// MaybeIdItem member definitions
//
///////////////////////////////////////////////////////////////////////

template <class T>
MaybeIdItem<T>::MaybeIdItem(T* master) :
  itsHandle(master),
  itsId(master != 0 ? ptrList().insert(master).id() : -1)
{}

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
	 }
}

template <class T>
void MaybeIdItem<T>::attemptRefresh() const {
  if ( !itsHandle.isValid() )
	 {
		try {
		  typename PtrList<T>::SharedPtr p = ptrList().getCheckedPtr(itsId);
		  itsHandle = p.handle();
		}
		catch (InvalidIdError&) { /* eat the exception */ }
	 }
}

static const char vcid_iditem_cc[] = "$Header$";
#endif // !IDITEM_CC_DEFINED
