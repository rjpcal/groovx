///////////////////////////////////////////////////////////////////////
//
// voidptrlist.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Sat Nov 20 23:58:42 1999
// written: Sat Mar  4 01:50:32 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef VOIDPTRLIST_H_DEFINED
#define VOIDPTRLIST_H_DEFINED

#ifndef ERROR_H_DEFINED
#include "error.h"
#endif

/**
 *
 * InvalidIdError is an exception class that will be thrown from
 * VoidPtrList if an attempt to use an invalid id is made in a checked
 * function.
 *
 **/

class InvalidIdError : public ErrorWithMsg {
public:
  ///
  InvalidIdError() : ErrorWithMsg() {}
  ///
  InvalidIdError(const string& msg) : ErrorWithMsg(msg) {}
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
  /** Return the void* at the index given by 'id'.  There is no
		range-check performed; this must be done by the client with
		isValidId(). */
  void* getVoidPtr(int id) const throw ();

  /** Like getVoidPtr(), but checks first if 'id' is a valid index,
		and throws an InvalidIdError if it is not. */
  void* getCheckedVoidPtr(int id) const throw (InvalidIdError);

  /** Add ptr at the next available location, and return the index
		where it was inserted. If necessary, the list will be expanded
		to make room for the ptr. The PtrList now assumes control of the
		memory management for the object *ptr. */
  int insertVoidPtr(void* ptr);

  /** Add obj at index 'id', destroying any the object was previously
		pointed to from that that location. The list will be expanded if
		'id' exceeds the size of the list. If id is < 0, the function
		returns without effect. */
  void insertVoidPtrAt(int id, void* ptr);

  /** This function will be called after every insertion into the
      VoidPtrList. The default implementation is a no-op. */
  virtual void afterInsertHook(int id, void* ptr);

  /** Must be overridden to free the memory pointed to by ptr. */
  virtual void destroyPtr(void* ptr) = 0;

protected:
  int& firstVacant();
  const int& firstVacant() const;

  void** voidVecBegin() const;
  void** voidVecEnd() const;
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
