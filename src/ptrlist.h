///////////////////////////////////////////////////////////////////////
//
// ptrlist.h
// Rob Peters
// created: Fri Apr 23 00:35:31 1999
// written: Sat Oct  7 20:07:11 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef PTRLIST_H_DEFINED
#define PTRLIST_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(DUMBPTR_H_DEFINED)
#include "dumbptr.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IOPTRLIST_H_DEFINED)
#include "ioptrlist.h"
#endif

///////////////////////////////////////////////////////////////////////
/**
 *
 * PtrList is templatized container that stores pointers and owns
 * (memory-manages) the objects to which the pointers refer. PtrList
 * is responsible for the memory management of all the objects to
 * which it holds pointers. The pointers are accessed by integer
 * indices into the PtrList. There are no operations on the capacity
 * of PtrList; any necessary resizing is done when necessary in an
 * insert call. PtrList is what Jeff Alger (in C++ for Real
 * Programmers) calls a "teenage mutable serializable indexed ninja
 * collection".
 *
 **/
///////////////////////////////////////////////////////////////////////

template <class T>
class PtrList : public IoPtrList {
public:
  /// Construct and reserve space for \a size objects.
  PtrList (int size);

  /** Virtual destructor calls \c clear(), according to the
      requirement of \c VoidPtrList, to ensure no memory
      leaks. Further subclasses of \c PtrList do not need to call \c
      clear() from their destructors. */
  virtual ~PtrList();

  //////////////
  // pointers //
  //////////////

  typedef T* Ptr;

  typedef DumbPtr<T> SharedPtr;

  ///////////////
  // accessors //
  ///////////////

public:
  /** Return the object at index \a id. No range check is performed on
      \a id, so the behavior is undefined if \a id is not a valid
      index into the list. If \a id is out of range, a segmentation
      fault may occur. If \a is in range, but is not an index for a
      valid object, the pointer returned may be null. */
  SharedPtr getPtr(int id) const throw () 
    {
		MasterPtrBase* voidPtr = VoidPtrList::getVoidPtr(id);
		MasterIoPtr* ioPtr = dynamic_cast<MasterIoPtr*>(voidPtr);
		if (ioPtr == 0) throw InvalidIdError("IO cast failed");
		T* t = dynamic_cast<T*>(ioPtr->ioPtr());
		if (t == 0) throw InvalidIdError("T cast failed");
		return SharedPtr(t);
	 }

  /** Returns the object at index \a id, after a check is performed to
      ensure that \a id is in range, and refers to a valid object. If
      the check fails, an \c InvalidIdError exception is thrown. */
  SharedPtr getCheckedPtr(int id) const throw (InvalidIdError)
	 {
		MasterPtrBase* voidPtr = VoidPtrList::getCheckedVoidPtr(id);
		MasterIoPtr* ioPtr = dynamic_cast<MasterIoPtr*>(voidPtr);
		if (ioPtr == 0) throw InvalidIdError("IO cast failed");
		T* t = dynamic_cast<T*>(ioPtr->ioPtr());
		if (t == 0) throw InvalidIdError("T cast failed");
		return SharedPtr(t);
	 }

  //////////////////
  // manipulators //
  //////////////////

#if 0
  /** Releases the object at index \a id. Ownership of the pointed-to
      object is transferred to the caller, and the list site at \a id
      becomes vacant. */
  Ptr release(int id) throw (InvalidIdError)
	 { return Ptr(castToT(VoidPtrList::releaseVoidPtr(id))); }
#endif

  /// Insert \a ptr into the list, and return its id.
  int insert(T* ptr)
	 { return VoidPtrList::insertVoidPtr(new MasterIoPtr(ptr)); }

  /** Insert \a ptr into the list at index \a id. If an object
      previously existed at index \a id, that object will be properly
      destroyed. */
  void insertAt(int id, T* ptr)
	 { VoidPtrList::insertVoidPtrAt(id, new MasterIoPtr(ptr)); }

protected:
  /** Reimplemented from \c IoPtrList to include "PtrList<T>" with \c
      \a T replaced with the the typename of the actual template argument. */
  virtual const char* alternateIoTags() const;
};

static const char vcid_ptrlist_h[] = "$Header$";
#endif // !PTRLIST_H_DEFINED
