///////////////////////////////////////////////////////////////////////
//
// voidptrlist.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Sat Nov 20 23:58:42 1999
// written: Sun Oct  8 16:25:52 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef VOIDPTRLIST_H_DEFINED
#define VOIDPTRLIST_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(ERROR_H_DEFINED)
#include "util/error.h"
#endif

class VoidPtrList;

/**
 *
 * InvalidIdError is an exception class that will be thrown from
 * VoidPtrList if an attempt to use an invalid id is made in a checked
 * function.
 *
 **/

class InvalidIdError : public ErrorWithMsg {
public:
  /// Default constructor.
  InvalidIdError();
  /// Construct with an informative message \a msg.
  InvalidIdError(const char* msg);
  /// Virtual destructor.
  virtual ~InvalidIdError();
};

///////////////////////////////////////////////////////////////////////
/**
 *
 * MasterPtrBase
 *
 **/
///////////////////////////////////////////////////////////////////////

class MasterPtrBase {
private:
  int itsRefCount;

  // These are disallowed since MasterPtrBase's should always be in
  // one-to-one correspondence with their pointee's.
  MasterPtrBase(const MasterPtrBase& other);
  MasterPtrBase& operator=(const MasterPtrBase& other);

public:
  MasterPtrBase();

  virtual ~MasterPtrBase();

  void incrRefCount();
  void decrRefCount();

  int refCount() const;

  virtual bool isValid() const = 0;

  virtual bool operator==(const MasterPtrBase& other) = 0;
};

///////////////////////////////////////////////////////////////////////
/**
 *
 * VoidPtrList provides a non-typesafe implementation of a container
 * that stores pointers and owns them. VoidPtrList delegates the
 * destruction of its contained objects to its subclasses by way of
 * destroyPtr().
 *
 **/
///////////////////////////////////////////////////////////////////////

class VoidPtrList {
protected:
  /// Construct a PtrList with an initial reserve capacity of 'size'
  VoidPtrList(int size);

public:
  ///
  virtual ~VoidPtrList();

  /** Returns the size of the internal array. The number returned also
		refers to the one-past-the-end index into the PtrList. */
  int capacity() const;

  /** Returns the number of filled sites in the PtrList. */
  int count() const;

  /** Returns true if 'id' is a valid index into a non-NULL T* in
		the PtrList, given its current size. */
  bool isValidId(int id) const;

  ///
  template <class Iterator>
  void insertValidIds(Iterator itr) const {
	 for (unsigned int i = 0, end = voidVecSize();
			i < end;
			++i) {
		if (isValidId(i)) 
		  *itr++ = i;
	 }
  }

  /** Destroys the object at index 'i', and resets the void* to NULL. */
  void remove(int id);

  /** Destroys all objects held by the list, and reset all void*'s to
      NULL. NOTE: A concrete subclass MUST call clear() from its
      destructor in order to avoid a memory leak. VoidPtrList cannot
      call clear() in its own destructor because clear() calls
      destroyPtr(), which is a pure virtual function. */
  void clear();

protected:
  /** Return the \c void* at the index given by \a id.  There is no
		range-check performed; this must be done by the client with
		\c isValidId(). */
  MasterPtrBase* getVoidPtr(int id) const throw ();

  /** Like \c getVoidPtr(), but checks first if \a id is a valid index,
		and throws an \c InvalidIdError if it is not. */
  MasterPtrBase* getCheckedVoidPtr(int id) const throw (InvalidIdError);

#if 0
  /** Releases the \c void* at the given index from the management of
		the \c VoidPtrList. Ownership of the pointed-to object is
		transferred to the caller, and the \c void* is removed from the
		\c VoidPtrList. */
  void* releaseVoidPtr(int id) throw (InvalidIdError);
#endif

  /** Add ptr at the next available location, and return the index
		where it was inserted. If necessary, the list will be expanded
		to make room for the ptr. The PtrList now assumes control of the
		memory management for the object *ptr. */
  int insertVoidPtr(MasterPtrBase* ptr);

  /** Add obj at index 'id', destroying any the object was previously
		pointed to from that that location. The list will be expanded if
		'id' exceeds the size of the list. If id is < 0, the function
		returns without effect. */
  void insertVoidPtrAt(int id, MasterPtrBase* ptr);

  /** This function will be called after every insertion into the
      VoidPtrList. The default implementation is a no-op. */
  virtual void afterInsertHook(int id, MasterPtrBase* ptr);

protected:
  int& firstVacant();
  const int& firstVacant() const;

  unsigned int voidVecSize() const;

  void voidVecResize(unsigned int new_size);

private:
  VoidPtrList(const VoidPtrList&);
  VoidPtrList& operator=(const VoidPtrList&);

  class Impl;
  Impl* const itsImpl;
};

static const char vcid_voidptrlist_h[] = "$Header$";
#endif // !VOIDPTRLIST_H_DEFINED
