///////////////////////////////////////////////////////////////////////
//
// iodb.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sun Nov 21 00:26:29 1999
// written: Tue Jun  5 10:41:43 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IODB_H_DEFINED
#define IODB_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(ERROR_H_DEFINED)
#include "util/error.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(UTILFWD_H_DEFINED)
#include "util/utilfwd.h"
#endif

/**
 *
 * InvalidIdError is an exception class that will be thrown from
 * IoDb if an attempt to use an invalid id is made in a checked
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
 * IoDb.
 *
 **/
///////////////////////////////////////////////////////////////////////

class IoDb {
private:
  static IoDb theInstance;

  /// Default constructor makes an empty list.
  IoDb();

public:
  /// Virtual destructor.
  virtual ~IoDb();

  /// Returns the singleton instance of IoDb.
  static IoDb& theDb();

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

	 Util::Object* getObject() const;
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

	 Util::Object* operator*() const { return getObject(); }
  };

  Iterator begin() const;
  Iterator end() const;

  IdIterator beginIds() const { return begin(); }
  IdIterator endIds() const { return end(); }

  PtrIterator beginPtrs() const { return begin(); }
  PtrIterator endPtrs() const { return end(); }

  template <class T>
  class CastingIterator {
	 Iterator itsItr;
	 const Iterator itsEnd;

	 void advanceToValid()
	 {
		while ((itsItr != itsEnd) && (dynamic_cast<T*>(itsItr.getObject())==0))
		  ++itsItr;
	 }

  public:
	 CastingIterator(const Iterator& begin) :
		itsItr(begin), itsEnd(IoDb::theDb().end()) { advanceToValid(); }

	 CastingIterator& operator++() { ++itsItr; advanceToValid(); return *this; }

	 bool operator==(const CastingIterator& other)
	   { return itsItr == other.itsItr; }

	 bool operator!=(const CastingIterator& other)
	   { return itsItr != other.itsItr; }

	 T* operator*() const
	   { return &(dynamic_cast<T&>(*itsItr.getObject())); }

	 T* operator->() const
	   { return operator*(); }
  };

  //
  // Collection interface
  //

  /** Returns the number of filled sites in the PtrList. */
  int count() const;

  /** Returns true if 'id' is a valid index into a non-NULL T* in
		the PtrList, given its current size. */
  bool isValidId(Util::UID id) const;

  /** If the object at \a id is unshared, removes reference to the
      object at index \a id, causing the object to be destroyed since
      it was unshared. If the object is shared, this operation throws
      an exception. */
  void remove(Util::UID id);

  /** Removes reference to the object at \a id. */
  void release(Util::UID id);

  /** Releases references to all unshared objects held by the
      list. Since the objects are unshared, they will be destroyed in
      the process. */
  void purge();

  /** Calls \c purge() repeatedly until no more items can be
      removed. This will get rid of items that were only referenced by
      other items in the list. */
  void clear();

  /** WARNING: should only be called during program exit. Does a full
      clear of all objects held by the IoDb. This breaks the usual
      semantics of IoDb, since it removes both shared and unshared
      objects. */
  void clearOnExit();

  /** Return the \c Util::Object* at the index given by \a id. Checks
		first if \a id is a valid index, and throws an \c InvalidIdError
		if it is not. */
  Util::Object* getCheckedPtrBase(Util::UID id) throw (InvalidIdError);

  /** Add ptr at the next available location, and return the index
		where it was inserted. If necessary, the list will be expanded
		to make room for the ptr. The PtrList now assumes control of the
		memory management for the object *ptr. */
  void insertPtrBase(Util::Object* ptr);

private:
  IoDb(const IoDb&);
  IoDb& operator=(const IoDb&);

  Impl* const itsImpl;
};

static const char vcid_iodb_h[] = "$Header$";
#endif // !IODB_H_DEFINED
