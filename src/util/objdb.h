///////////////////////////////////////////////////////////////////////
//
// ioptrlist.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Sun Nov 21 00:26:29 1999
// written: Fri Oct 27 16:30:38 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IOPTRLIST_H_DEFINED
#define IOPTRLIST_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IO_H_DEFINED)
#include "io/io.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(ERROR_H_DEFINED)
#include "util/error.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IDITEM_H_DEFINED)
#include "iditem.h"
#endif

namespace IO { class IoObject; }

/**
 *
 * InvalidIdError is an exception class that will be thrown from
 * IoPtrList if an attempt to use an invalid id is made in a checked
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
 * IoPtrList.
 *
 **/
///////////////////////////////////////////////////////////////////////

class IoPtrList : public virtual IO::IoObject {
private:
  static IoPtrList theInstance;

  /// Default constructor makes an empty list.
  IoPtrList();

public:
  /// Virtual destructor.
  virtual ~IoPtrList();

  static IoPtrList& theList();

  //
  // IO interface
  //

  virtual IO::VersionId serialVersionId() const;
  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

  class Impl;
  class ItrImpl;

  //
  // Iterators
  //

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
	 IoPtrList* itsList;
  public:
	 Inserter(IoPtrList* list_) : itsList(list_) {}

	 Inserter& operator=(IO::IoObject* obj)
		{ itsList->insertPtrBase(obj); return *this; }

	 Inserter& operator*() { return *this; }
	 Inserter& operator++() { return *this; }
	 Inserter& operator++(int) { return *this; }
  };

  Inserter inserter() { return Inserter(this); }

  //
  // Collection interface
  //

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

  /** Return the \c IO::IoObject* at the index given by \a id. Checks
		first if \a id is a valid index, and throws an \c InvalidIdError
		if it is not. */
  IO::IoObject* getCheckedPtrBase(int id) const throw (InvalidIdError);

  /** Add ptr at the next available location, and return the index
		where it was inserted. If necessary, the list will be expanded
		to make room for the ptr. The PtrList now assumes control of the
		memory management for the object *ptr. */
  int insertPtrBase(IO::IoObject* ptr);

public:

  template <class T>
  IdItem<T> getCheckedIoPtr(int id) const
    {
		IO::IoObject* voidPtr = getCheckedPtrBase(id);

		// cast as reference to force an exception on error
		T& t = dynamic_cast<T&>(*voidPtr);

		return IdItem<T>(&t);
	 }

  template <class T>
  void insertIo(T* master)
    {
		insertPtrBase(master);
	 }

private:
  IoPtrList(const IoPtrList&);
  IoPtrList& operator=(const IoPtrList&);

  Impl* const itsImpl;
};

static const char vcid_ioptrlist_h[] = "$Header$";
#endif // !IOPTRLIST_H_DEFINED
