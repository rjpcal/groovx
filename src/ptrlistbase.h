///////////////////////////////////////////////////////////////////////
//
// ptrlistbase.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Sat Nov 20 23:58:42 1999
// written: Wed Oct 25 13:48:07 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef PTRLISTBASE_H_DEFINED
#define PTRLISTBASE_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(ERROR_H_DEFINED)
#include "util/error.h"
#endif

namespace IO { class IoObject; }

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
public:

  class Impl;
  class ItrImpl;

  class IdIterator {
  private:
	 ItrImpl* itsImpl;
  public:
	 IdIterator(ItrImpl* impl);
	 ~IdIterator();
	 IdIterator(const IdIterator& other);
	 IdIterator& operator=(const IdIterator& other);

	 bool operator==(const IdIterator& other) const;

	 bool operator!=(const IdIterator& other) const
		{ return !(this->operator==(other)); }

	 IdIterator& operator++();

	 int operator*() const;

	 IO::IoObject* getObject() const;
  };

  IdIterator beginIds() const;
  IdIterator endIds() const;

protected:
  /// Construct a PtrList with an initial reserve capacity of 'size'
  PtrListBase(int size);

public:
  ///
  virtual ~PtrListBase();

  /** Returns the size of the internal array. The number returned also
		refers to the one-past-the-end index into the PtrList. */
  virtual int capacity() const;

  /** Returns the number of filled sites in the PtrList. */
  virtual int count() const;

  /** Returns true if 'id' is a valid index into a non-NULL T* in
		the PtrList, given its current size. */
  virtual bool isValidId(int id) const;

  /** Insert all of the valid id's in the list using the insert
      iterator \a itr. **/
  template <class Iterator>
  void insertValidIds(Iterator itr) const {
	 for(IdIterator id_itr = beginIds(), end = endIds();
		  id_itr != end;
		  ++id_itr)
		{
		  *itr++ = *id_itr;
		}
  }

  /** If the object at \a id is unshared, removes reference to the
      object at index \a id, causing the object to be destroyed since
      it was unshared. If the object is shared, this operation throws
      an exception. */
  virtual void remove(int id);

  /** Removes reference to the object at \a id. */
  virtual void release(int id);

  /** Releases references to all unshared objects held by the
      list. Since the objects are unshared, they will be destroyed in
      the process. */
  virtual void clear();

protected:
  /** Return the \c IO::IoObject* at the index given by \a id. Checks
		first if \a id is a valid index, and throws an \c InvalidIdError
		if it is not. */
  virtual IO::IoObject* getCheckedPtrBase(int id) const throw (InvalidIdError);

  /** Add ptr at the next available location, and return the index
		where it was inserted. If necessary, the list will be expanded
		to make room for the ptr. The PtrList now assumes control of the
		memory management for the object *ptr. */
  virtual int insertPtrBase(IO::IoObject* ptr);

  /** Subclasses override to throw an exception if \a ptr is of the
      wrong type to be in the list. */
  virtual void ensureCorrectType(const IO::IoObject* ptr) const = 0;

private:
  PtrListBase(const PtrListBase&);
  PtrListBase& operator=(const PtrListBase&);

  Impl* const itsImpl;
};

static const char vcid_ptrlistbase_h[] = "$Header$";
#endif // !PTRLISTBASE_H_DEFINED
