///////////////////////////////////////////////////////////////////////
//
// ptrlist.h
// Rob Peters
// created: Fri Apr 23 00:35:31 1999
// written: Sun Oct 22 15:59:10 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef PTRLIST_H_DEFINED
#define PTRLIST_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IOPTRLIST_H_DEFINED)
#include "ioptrlist.h"
#endif

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
 * PtrList is templatized container that stores reference counted
 * smart pointers. Objects are accessed by integer indices into
 * the PtrList. There are no operations on the capacity of PtrList;
 * any necessary resizing is done when necessary in an insert
 * call. PtrList is what Jeff Alger (in C++ for Real Programmers)
 * calls a "teenage mutable serializable indexed ninja collection".
 *
 **/
///////////////////////////////////////////////////////////////////////

template <class T>
class PtrList : public IoPtrList {
public:
  /// Construct and reserve space for \a size objects.
  PtrList (int size);

  /// Virtual destructor.
  virtual ~PtrList();

  //////////////
  // pointers //
  //////////////

  typedef T* Ptr;

  /// A reference-counted handle to type T.
  typedef ItemWithId<T> SharedPtr;

  ///////////////
  // accessors //
  ///////////////

public:
  /** Return the object at index \a id. No range check is performed on
      \a id, so the behavior is undefined if \a id is not a valid
      index into the list. If \a id is out of range, a segmentation
      fault may occur. If \a is in range, but is not an index for a
      valid object, the pointer returned may be null. */
  SharedPtr getPtr(int id) const;

  /** Returns the object at index \a id, after a check is performed to
      ensure that \a id is in range, and refers to a valid object. If
      the check fails, an \c InvalidIdError exception is thrown. */
  SharedPtr getCheckedPtr(int id) const;

  //////////////////
  // manipulators //
  //////////////////

  /// Insert \a ptr into the list, and return its id.
  SharedPtr insert(T* master);

  /** Insert \a ptr into the list at index \a id. An exception will be
      thrown if an object already exists at index \a id. */
  void insertAt(int id, T* master);

  /// Insert \a handle into the list, and return its id.
  SharedPtr insert(PtrHandle<T> handle)
	 { return insert(handle.get()); }

  /** Insert \a handle into the list at index \a id. An exception will be
      thrown if an object already exists at index \a id. */
  void insertAt(int id, PtrHandle<T> handle)
	 { PtrListBase::insertPtrBaseAt(id, handle.get()); }

  /// Insert \a item into the list, and return its id.
  SharedPtr insert(ItemWithId<T> item)
	 { return insert(item.handle().get()); }

  /** Insert \a ptr into the list at index \a id. An exception will be
      thrown if an object already exists at index \a id. */
  void insertAt(int id, ItemWithId<T> item)
	 { PtrListBase::insertPtrBaseAt(id, item.handle().get()); }
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

  // Default destructor, copy constructor, operator=() are fine

        T* operator->()       { refresh(); return itsHandle.get(); }
  const T* operator->() const { refresh(); return itsHandle.get(); }
        T& operator*()        { refresh(); return *(itsHandle.get()); }
  const T& operator*()  const { refresh(); return *(itsHandle.get()); }

        T* get()              { refresh(); return itsHandle.get(); }
  const T* get()        const { refresh(); return itsHandle.get(); }

  void refresh() const
    {
		if ( !itsHandle.isValid() )
		  {
			 typename PtrList<T>::SharedPtr p = ptrList().getCheckedPtr(itsId);
			 itsHandle = p.handle();
		  }
	 }

  bool isValid() const { return itsHandle.isValid(); }

  NullablePtrHandle<T> handle() const { refresh(); return itsHandle; }
  int id() const { return itsId; }
};

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

static const char vcid_ptrlist_h[] = "$Header$";
#endif // !PTRLIST_H_DEFINED
