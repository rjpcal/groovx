///////////////////////////////////////////////////////////////////////
//
// itemwithid.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Oct 23 11:41:23 2000
// written: Mon Oct 23 13:10:01 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef ITEMWITHID_H_DEFINED
#define ITEMWITHID_H_DEFINED


#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(PTRHANDLE_H_DEFINED)
#include "util/ptrhandle.h"
#endif

template <class T> class PtrList;

///////////////////////////////////////////////////////////////////////
/**
 *
 * ItemWithId<T> is a wrapper of a PtrHandle<T> along with an integer
 * index from a PtrList<T>.
 *
 **/
///////////////////////////////////////////////////////////////////////

template <class T>
class ItemWithId {
private:
  static PtrList<T>& ptrList();

  PtrHandle<T> itsHandle;
  const int itsId;

public:

  ItemWithId(T* master, int id_) : itsHandle(master), itsId(id_) {}
  ItemWithId(PtrHandle<T> item_, int id_) : itsHandle(item_), itsId(id_) {}

  /** A symbol to pass to constructors indicating that the item should
		be inserted into an appropriate PtrList. */
  enum Insert { INSERT };

  ItemWithId(T* ptr, Insert /*dummy param*/);
  ItemWithId(PtrHandle<T> item, Insert /*dummy param*/);

  // Default destructor, copy constructor, operator=() are fine

        T* operator->()       { return itsHandle.get(); }
  const T* operator->() const { return itsHandle.get(); }
        T& operator*()        { return *(itsHandle.get()); }
  const T& operator*()  const { return *(itsHandle.get()); }

        T* get()              { return itsHandle.get(); }
  const T* get()        const { return itsHandle.get(); }

  PtrHandle<T> handle() const { return itsHandle; }
  int id() const { return itsId; }
};


///////////////////////////////////////////////////////////////////////
/**
 *
 * NullableItemWithId<T> is a wrapper of a NullablePtrHandle<T> along
 * with an integer index from a PtrList<T>.
 *
 **/
///////////////////////////////////////////////////////////////////////

template <class T>
class NullableItemWithId {
private:
  static PtrList<T>& ptrList();

  mutable NullablePtrHandle<T> itsHandle;
  int itsId;

public:
  explicit NullableItemWithId(int id_) :
	 itsHandle(0), itsId(id_) {}

  NullableItemWithId(T* master, int id_) :
	 itsHandle(master), itsId(id_) {}

  NullableItemWithId(PtrHandle<T> item_, int id_) :
	 itsHandle(item_), itsId(id_) {}

  NullableItemWithId(NullablePtrHandle<T> item_, int id_) :
	 itsHandle(item_), itsId(id_) {}

  NullableItemWithId(const ItemWithId<T> other) :
	 itsHandle(other.handle()), itsId(other.id()) {}

  // These will cause the item to be inserted into the relevant list.
  NullableItemWithId(T* master);
  NullableItemWithId(PtrHandle<T> item);

  // Default destructor, copy constructor, operator=() are fine

        T* operator->()       { refresh(); return itsHandle.get(); }
  const T* operator->() const { refresh(); return itsHandle.get(); }
        T& operator*()        { refresh(); return *(itsHandle.get()); }
  const T& operator*()  const { refresh(); return *(itsHandle.get()); }

        T* get()              { refresh(); return itsHandle.get(); }
  const T* get()        const { refresh(); return itsHandle.get(); }

  void refresh() const;

  bool isValid() const { return itsHandle.isValid(); }

  NullablePtrHandle<T> handle() const { refresh(); return itsHandle; }
  int id() const { return itsId; }
};

static const char vcid_itemwithid_h[] = "$Header$";
#endif // !ITEMWITHID_H_DEFINED
