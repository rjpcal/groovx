///////////////////////////////////////////////////////////////////////
//
// objdb.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sun Nov 21 00:26:29 1999
// written: Wed Sep 25 18:59:09 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef OBJDB_H_DEFINED
#define OBJDB_H_DEFINED

#include "util/error.h"
#include "util/iter.h"
#include "util/uid.h"

namespace Util
{
  class Object;
  template <class T> class SoftRef;
};

/**
 *
 * InvalidIdError is an exception class that will be thrown from
 * ObjDb if an attempt to use an invalid id is made in a checked
 * function.
 *
 **/

class InvalidIdError : public Util::Error
{
public:
  InvalidIdError(const fstring& msg) : Util::Error(msg) {};

  /// Virtual destructor.
  virtual ~InvalidIdError();
};

///////////////////////////////////////////////////////////////////////
/**
 *
 * ObjDb is a database for storing Util::Object's, which can be
 * accessed by the objects' Util::UID's. Most clients will not need to
 * use the ObjDb directly, but can instead manage Util::Object's
 * indirectly with the Util::Ref and Util::SoftRef smart pointers.
 *
 **/
///////////////////////////////////////////////////////////////////////

class ObjDb
{
protected:
  /// Default constructor makes an empty list.
  ObjDb();

public:
  /// Virtual destructor.
  virtual ~ObjDb();

  /// Returns the singleton instance of ObjDb.
  static ObjDb& theDb();

  class Impl;
  class ItrImpl;

  //
  // Iterators
  //

  typedef Util::FwdIter<const Util::SoftRef<Util::Object> > Iterator;

  Iterator objects() const;

  /// A filtering iterator class; only exposes objects matching a given type.
  template <class T>
  class CastingIterator
  {
    Iterator itsItr;

    void advanceToValid()
    {
      while (!itsItr.atEnd() && (dynamic_cast<T*>((*itsItr).getWeak())==0))
        ++itsItr;
    }

  public:
    CastingIterator(const Iterator& begin) : itsItr(begin)
    { advanceToValid(); }

    CastingIterator& operator++() { ++itsItr; advanceToValid(); return *this; }

    bool atEnd() const { return itsItr.atEnd(); }
    bool isValid() const { return itsItr.isValid(); }

    T* operator*() const { return &(dynamic_cast<T&>(**itsItr)); }

    T* operator->() const { return operator*(); }
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
      clear of all objects held by the ObjDb. This breaks the usual
      semantics of ObjDb, since it removes both shared and unshared
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
  ObjDb(const ObjDb&);
  ObjDb& operator=(const ObjDb&);

  Impl* const itsImpl;
};

static const char vcid_objdb_h[] = "$Header$";
#endif // !OBJDB_H_DEFINED
