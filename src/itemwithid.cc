///////////////////////////////////////////////////////////////////////
//
// itemwithid.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Oct 23 11:42:18 2000
// written: Wed Oct 25 18:18:05 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef ITEMWITHID_CC_DEFINED
#define ITEMWITHID_CC_DEFINED

#include "itemwithid.h"

#include "util/error.h"

///////////////////////////////////////////////////////////////////////
//
// ItemWithId member definitions
//
///////////////////////////////////////////////////////////////////////

template <class T>
ItemWithId<T>::ItemWithId(int id) :
  itsHandle(ptrList().getCheckedPtr(id).handle()),
  itsId(id)
{}

template <class T>
ItemWithId<T>::ItemWithId(T* ptr, Insert /*dummy param*/) :
  itsHandle(ptr),
  itsId(ptrList().insert(itsHandle).id())
{}

template <class T>
ItemWithId<T>::ItemWithId(PtrHandle<T> item, Insert /*dummy param*/) :
  itsHandle(item),
  itsId(ptrList().insert(itsHandle).id())
{}

template <class T>
ItemWithId<T>& ItemWithId<T>::operator=(T* new_master)
{
  itsHandle = PtrHandle<T>(new_master);
  itsId = ptrList().insert(itsHandle).id();
}

///////////////////////////////////////////////////////////////////////
//
// NullableItemWithId member definitions
//
///////////////////////////////////////////////////////////////////////

template <class T>
NullableItemWithId<T>::NullableItemWithId(T* master) :
  itsHandle(master),
  itsId(master != 0 ? ptrList().insert(master).id() : -1)
{}

template <class T>
NullableItemWithId<T>::NullableItemWithId(PtrHandle<T> item) :
  itsHandle(item),
  itsId(ptrList().insert(itsHandle.get()).id())
{}

template <class T>
void NullableItemWithId<T>::refresh() const {
  if ( !itsHandle.isValid() )
	 {
		typename PtrList<T>::SharedPtr p = ptrList().getCheckedPtr(itsId);
		itsHandle = p.handle();
	 }
}

template <class T>
void NullableItemWithId<T>::attemptRefresh() const {
  if ( !itsHandle.isValid() )
	 {
		try {
		  typename PtrList<T>::SharedPtr p = ptrList().getCheckedPtr(itsId);
		  itsHandle = p.handle();
		}
		catch (InvalidIdError&) { /* eat the exception */ }
	 }
}

static const char vcid_itemwithid_cc[] = "$Header$";
#endif // !ITEMWITHID_CC_DEFINED
