///////////////////////////////////////////////////////////////////////
//
// ptrlistbase.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Sat Nov 20 23:58:42 1999
// written: Wed Oct 25 14:40:34 2000
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

  class Iterator {
  private:
	 ItrImpl* itsImpl;
  public:
	 Iterator(ItrImpl* impl);
	 ~Iterator();
	 Iterator(const Iterator& other);
	 Iterator& operator=(const Iterator& other);

	 bool operator==(const Iterator& other) const;

	 bool operator!=(const Iterator& other) const
		{ return !(this->operator==(other)); }

	 Iterator& operator++();

	 int getId() const;

	 IO::IoObject* getObject() const;
  };

  class IdIterator : public Iterator {
	 IdIterator();
  public:
	 IdIterator(const Iterator& other) : Iterator(other) {}

	 int operator*() const { return getId(); }
  };

  class PtrIterator : public Iterator {
	 PtrIterator();
  public:
	 PtrIterator(const Iterator& other) : Iterator(other) {}

	 IO::IoObject* operator*() const { return getObject(); }
  };

  Iterator begin() const;
  Iterator end() const;

  IdIterator beginIds() const { return begin(); }
  IdIterator endIds() const { return end(); }

  PtrIterator beginPtrs() const { return begin(); }
  PtrIterator endPtrs() const { return end(); }

  class Inserter;
  friend class Inserter;

  class Inserter {
	 PtrListBase* itsList;
  public:
	 Inserter(PtrListBase* list_) : itsList(list_) {}

	 Inserter& operator=(IO::IoObject* obj)
		{ itsList->insertPtrBase(obj); return *this; }

	 Inserter& operator*() { return *this; }
	 Inserter& operator++() { return *this; }
	 Inserter& operator++(int) { return *this; }
  };

  Inserter inserter() { return Inserter(this); }

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
  /** Return the \c IO::IoObject* at the index given by \a id. Checks
		first if \a id is a valid index, and throws an \c InvalidIdError
		if it is not. */
  IO::IoObject* getCheckedPtrBase(int id) const throw (InvalidIdError);

  /** Add ptr at the next available location, and return the index
		where it was inserted. If necessary, the list will be expanded
		to make room for the ptr. The PtrList now assumes control of the
		memory management for the object *ptr. */
  int insertPtrBase(IO::IoObject* ptr);

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
