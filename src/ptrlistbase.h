///////////////////////////////////////////////////////////////////////
//
// ptrlistbase.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Sat Nov 20 23:58:42 1999
// written: Tue Oct 24 13:44:41 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef PTRLISTBASE_H_DEFINED
#define PTRLISTBASE_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(ERROR_H_DEFINED)
#include "util/error.h"
#endif

class RefCounted;

/**
 *
 * InvalidIdError is an exception class that will be thrown from
 * PtrListBase if an attempt to use an invalid id is made in a checked
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
 * PtrListBase provides a non-typesafe implementation of a container
 * that stores pointers and owns them. PtrListBase delegates the
 * destruction of its contained objects to its subclasses by way of
 * destroyPtr().
 *
 **/
///////////////////////////////////////////////////////////////////////

class PtrListBase {
protected:
  /// Construct a PtrList with an initial reserve capacity of 'size'
  PtrListBase(int size);

public:
  ///
  virtual ~PtrListBase();

  /** Returns the size of the internal array. The number returned also
		refers to the one-past-the-end index into the PtrList. */
  int capacity() const;

  /** Returns the number of filled sites in the PtrList. */
  int count() const;

  /** Returns true if 'id' is a valid index into a non-NULL T* in
		the PtrList, given its current size. */
  bool isValidId(int id) const;

  /** Insert all of the valid id's in the list using the insert
      iterator \a itr. **/
  template <class Iterator>
  void insertValidIds(Iterator itr) const {
	 for (unsigned int i = 0, end = baseVecSize();
			i < end;
			++i) {
		if (isValidId(i)) 
		  *itr++ = i;
	 }
  }

  /** If the object at \a id is unshared, removes reference to the
      object at index \a id, causing the object to be destroyed since
      it was unshared. If the object is shared, this operation throws
      an exception. */
  void remove(int id);

  /** Removes reference to the object at \a id. */
  void release(int id);

  /** Releases references to all unshared objects held by the
      list. Since the objects are unshared, they will be destroyed in
      the process. */
  void clear();

protected:
  /** Return the \c void* at the index given by \a id.  There is no
		range-check performed; this must be done by the client with
		\c isValidId(). */
  RefCounted* getPtrBase(int id) const throw ();

  /** Like \c getPtrBase(), but checks first if \a id is a valid index,
		and throws an \c InvalidIdError if it is not. */
  RefCounted* getCheckedPtrBase(int id) const throw (InvalidIdError);

  /** Add ptr at the next available location, and return the index
		where it was inserted. If necessary, the list will be expanded
		to make room for the ptr. The PtrList now assumes control of the
		memory management for the object *ptr. */
  int insertPtrBase(RefCounted* ptr);

protected:
  int& firstVacant();
  const int& firstVacant() const;

  unsigned int baseVecSize() const;

  void baseVecResize(unsigned int new_size);

private:
  PtrListBase(const PtrListBase&);
  PtrListBase& operator=(const PtrListBase&);

  class Impl;
  Impl* const itsImpl;
};

static const char vcid_ptrlistbase_h[] = "$Header$";
#endif // !PTRLISTBASE_H_DEFINED
