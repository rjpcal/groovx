///////////////////////////////////////////////////////////////////////
//
// ptrlist.h
// Rob Peters
// created: Fri Apr 23 00:35:31 1999
// written: Mon Oct 23 11:42:33 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef PTRLIST_H_DEFINED
#define PTRLIST_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IOPTRLIST_H_DEFINED)
#include "ioptrlist.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(ITEMWITHID_H_DEFINED)
#include "itemwithid.h"
#endif

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

static const char vcid_ptrlist_h[] = "$Header$";
#endif // !PTRLIST_H_DEFINED
