///////////////////////////////////////////////////////////////////////
//
// ptrlist.h
// Rob Peters
// created: Fri Apr 23 00:35:31 1999
// written: Wed Oct 25 14:41:14 2000
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

  /// A reference-counted handle to type T.
  typedef ItemWithId<T> SharedPtr;

  friend class ItemWithId<T>;
  friend class NullableItemWithId<T>;

protected:
  virtual void ensureCorrectType(const IO::IoObject* ptr) const;

private:
  /** Returns the object at index \a id, after a check is performed to
      ensure that \a id is in range, and refers to a valid object. If
      the check fails, an \c InvalidIdError exception is thrown. */
  SharedPtr getCheckedPtr(int id) const;

  /// Insert \a ptr into the list, and return its id.
  SharedPtr insert(T* master);

  /// Insert \a handle into the list, and return its id.
  SharedPtr insert(PtrHandle<T> handle)
	 { return insert(handle.get()); }

  /// Insert \a item into the list, and return its id.
  SharedPtr insert(ItemWithId<T> item)
	 { return insert(item.handle().get()); }
};

static const char vcid_ptrlist_h[] = "$Header$";
#endif // !PTRLIST_H_DEFINED
