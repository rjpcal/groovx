///////////////////////////////////////////////////////////////////////
//
// itemwithid.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Oct 23 11:42:18 2000
// written: Mon Oct 23 11:43:29 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef ITEMWITHID_CC_DEFINED
#define ITEMWITHID_CC_DEFINED

#include "itemwithid.h"


///////////////////////////////////////////////////////////////////////
//
// ItemWithId member definitions
//
///////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////
//
// NullableItemWithId member definitions
//
///////////////////////////////////////////////////////////////////////

template <class T>
NullableItemWithId<T>::refresh() const {
  if ( !itsHandle.isValid() )
	 {
		typename PtrList<T>::SharedPtr p = ptrList().getCheckedPtr(itsId);
		itsHandle = p.handle();
	 }
}

static const char vcid_itemwithid_cc[] = "$Header$";
#endif // !ITEMWITHID_CC_DEFINED
