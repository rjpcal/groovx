///////////////////////////////////////////////////////////////////////
//
// ptrlist.h
// Rob Peters
// created: Fri Apr 23 00:35:31 1999
// written: Sun Oct  8 17:02:05 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef PTRLIST_H_DEFINED
#define PTRLIST_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IOPTRLIST_H_DEFINED)
#include "ioptrlist.h"
#endif

template <class T>
class MasterPtr : public MasterIoPtr {
private:
  T* itsPtr;

public:
  explicit MasterPtr(T* ptr);
  virtual ~MasterPtr();

  T* getPtr() { return itsPtr; }

  virtual IO::IoObject* ioPtr() const;

  virtual bool isValid() const;

  virtual bool operator==(const MasterPtrBase& other);
};

template <class T>
class PtrHandle {
public:
  explicit PtrHandle(MasterPtr<T>* master) : itsMaster(master)
  {
	 if (master == 0) throw ErrorWithMsg("MasterPtr<T> was null");
	 itsMaster->incrRefCount();
  }

  ~PtrHandle()
    { itsMaster->decrRefCount(); }

  PtrHandle(const PtrHandle& other) : itsMaster(other.itsMaster)
    { itsMaster->incrRefCount(); }

  PtrHandle& operator=(const PtrHandle& other)
    {
		PtrHandle otherCopy(other);
		this->swap(otherCopy);
		return *this;
	 }

  MasterPtr<T>* masterPtr()
    { return itsMaster; }

        T* operator->()       { return (itsMaster->getPtr()); }
  const T* operator->() const { return (itsMaster->getPtr()); }
        T& operator*()        { return *(itsMaster->getPtr()); }
  const T& operator*()  const { return *(itsMaster->getPtr()); }

        T* get()              { return (itsMaster->getPtr()); }
  const T* get()        const { return (itsMaster->getPtr()); }

private:
  void swap(PtrHandle& other)
    {
		MasterPtr<T>* otherMaster = other.itsMaster;
		other.itsMaster = this->itsMaster;
		this->itsMaster = otherMaster;
	 }

  MasterPtr<T>* itsMaster;
};

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

  typedef PtrHandle<T> SharedPtr;

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
		return SharedPtr(&tPtr);
	 }

  /** Returns the object at index \a id, after a check is performed to
      ensure that \a id is in range, and refers to a valid object. If
      the check fails, an \c InvalidIdError exception is thrown. */
  SharedPtr getCheckedPtr(int id) const
	 {
		MasterPtrBase* voidPtr = VoidPtrList::getCheckedVoidPtr(id);
		// cast as reference to force an exception on error
		MasterPtr<T>& tPtr = dynamic_cast<MasterPtr<T>&>(*voidPtr);
		return SharedPtr(&tPtr);
	 }

  //////////////////
  // manipulators //
  //////////////////

  /// Insert \a ptr into the list, and return its id.
  int insert(T* ptr)
	 { return VoidPtrList::insertVoidPtr(new MasterPtr<T>(ptr)); }

  /** Insert \a ptr into the list at index \a id. If an object
      previously existed at index \a id, that object will be properly
      destroyed. */
  void insertAt(int id, T* ptr)
	 { VoidPtrList::insertVoidPtrAt(id, new MasterPtr<T>(ptr)); }

  /// Insert \a ptr into the list, and return its id.
  int insert(MasterPtr<T>* master)
	 { return VoidPtrList::insertVoidPtr(master); }

  /** Insert \a ptr into the list at index \a id. If an object
      previously existed at index \a id, that object will be properly
      destroyed. */
  void insertAt(int id, MasterPtr<T>* master)
	 { VoidPtrList::insertVoidPtrAt(id, master); }

  /// Insert \a ptr into the list, and return its id.
  int insert(PtrHandle<T> handle)
	 { return VoidPtrList::insertVoidPtr(handle.masterPtr()); }

  /** Insert \a ptr into the list at index \a id. If an object
      previously existed at index \a id, that object will be properly
      destroyed. */
  void insertAt(int id, PtrHandle<T> handle)
	 { VoidPtrList::insertVoidPtrAt(id, handle.masterPtr()); }

protected:
  /** Reimplemented from \c IoPtrList to include "PtrList<T>" with \c
      \a T replaced with the the typename of the actual template argument. */
  virtual const char* alternateIoTags() const;

  virtual MasterIoPtr* makeMasterIoPtr(IO::IoObject* obj) const;
};

static const char vcid_ptrlist_h[] = "$Header$";
#endif // !PTRLIST_H_DEFINED
