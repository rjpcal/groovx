///////////////////////////////////////////////////////////////////////
//
// ptrlist.h
// Rob Peters
// created: Fri Apr 23 00:35:31 1999
// written: Mon Oct  9 11:42:57 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef PTRLIST_H_DEFINED
#define PTRLIST_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IOPTRLIST_H_DEFINED)
#include "ioptrlist.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(MASTERPTR_H_DEFINED)
#include "masterptr.h"
#endif

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
  PtrHandle<T> itsItem;
  const int itsId;

public:
  ItemWithId(MasterPtr<T>* master, int id_) : itsItem(master), itsId(id_) {}
  ItemWithId(PtrHandle<T> item_, int id_) : itsItem(item_), itsId(id_) {}

  // Default destructor, copy constructor, operator=() are fine

        T* operator->()       { return (itsItem.masterPtr()->getPtr()); }
  const T* operator->() const { return (itsItem.masterPtr()->getPtr()); }
        T& operator*()        { return *(itsItem.masterPtr()->getPtr()); }
  const T& operator*()  const { return *(itsItem.masterPtr()->getPtr()); }

        T* get()              { return (itsItem.masterPtr()->getPtr()); }
  const T* get()        const { return (itsItem.masterPtr()->getPtr()); }

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
  SharedPtr getPtr(int id) const
    {
		MasterPtrBase* voidPtr = VoidPtrList::getVoidPtr(id);
		// cast as reference to force an exception on error
		MasterPtr<T>& tPtr = dynamic_cast<MasterPtr<T>&>(*voidPtr);
		return SharedPtr(&tPtr, id);
	 }

  /** Returns the object at index \a id, after a check is performed to
      ensure that \a id is in range, and refers to a valid object. If
      the check fails, an \c InvalidIdError exception is thrown. */
  SharedPtr getCheckedPtr(int id) const
	 {
		MasterPtrBase* voidPtr = VoidPtrList::getCheckedVoidPtr(id);
		// cast as reference to force an exception on error
		MasterPtr<T>& tPtr = dynamic_cast<MasterPtr<T>&>(*voidPtr);
		return SharedPtr(&tPtr, id);
	 }

  //////////////////
  // manipulators //
  //////////////////

  /// Insert \a ptr into the list, and return its id.
  int insert(T* ptr)
	 { return VoidPtrList::insertVoidPtr(new MasterPtr<T>(ptr)); }

  /** Insert \a ptr into the list at index \a id. An exception will be
      thrown if an object already exists at index \a id. */
  void insertAt(int id, T* ptr)
	 { VoidPtrList::insertVoidPtrAt(id, new MasterPtr<T>(ptr)); }

  /// Insert \a ptr into the list, and return its id.
  int insert(MasterPtr<T>* master)
	 { return VoidPtrList::insertVoidPtr(master); }

  /** Insert \a ptr into the list at index \a id. An exception will be
      thrown if an object already exists at index \a id. */
  void insertAt(int id, MasterPtr<T>* master)
	 { VoidPtrList::insertVoidPtrAt(id, master); }

  /// Insert \a ptr into the list, and return its id.
  int insert(PtrHandle<T> handle)
	 { return VoidPtrList::insertVoidPtr(handle.masterPtr()); }

  /** Insert \a ptr into the list at index \a id. An exception will be
      thrown if an object already exists at index \a id. */
  void insertAt(int id, PtrHandle<T> handle)
	 { VoidPtrList::insertVoidPtrAt(id, handle.masterPtr()); }

protected:
  /** Reimplemented from \c IoPtrList to include "PtrList<T>" with \c
      \a T replaced with the the typename of the actual template argument. */
  virtual const char* alternateIoTags() const;

  /** Reimplemented from \c IoPtrList to return an MasterPtr<T>* that
      points to \a obj. */
  virtual MasterIoPtr* makeMasterIoPtr(IO::IoObject* obj) const;
};

static const char vcid_ptrlist_h[] = "$Header$";
#endif // !PTRLIST_H_DEFINED
