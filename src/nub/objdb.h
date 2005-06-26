///////////////////////////////////////////////////////////////////////
//
// objdb.h
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Sun Nov 21 00:26:29 1999
// commit: $Id$
// $HeadURL$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef GROOVX_NUB_OBJDB_H_UTC20050626084019_DEFINED
#define GROOVX_NUB_OBJDB_H_UTC20050626084019_DEFINED

#include "nub/ref.h"
#include "nub/uid.h"

#include "rutz/error.h"
#include "rutz/iter.h"

namespace Nub
{
  class Object;
  class ObjDb;
  class InvalidIdError;
  template <class T> class SoftRef;
}

/**
 *
 * Nub::InvalidIdError is an exception class that will be thrown from
 * ObjDb if an attempt to use an invalid id is made in a checked
 * function.
 *
 **/

class Nub::InvalidIdError : public rutz::error
{
public:
  /// Constructor.
  InvalidIdError(Nub::UID id, const rutz::file_pos& pos);

  /// Virtual destructor.
  virtual ~InvalidIdError() throw();
};

///////////////////////////////////////////////////////////////////////
/**
 *
 * ObjDb is a database for storing Nub::Object's, which can be
 * accessed by the objects' Nub::UID's. Most clients will not need to
 * use the ObjDb directly, but can instead manage Nub::Object's
 * indirectly with the Nub::Ref and Nub::SoftRef smart pointers.
 *
 **/
///////////////////////////////////////////////////////////////////////

class Nub::ObjDb
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

  typedef rutz::fwd_iter<const Nub::SoftRef<Nub::Object> > Iterator;

  Iterator objects() const;

  /// A filtering iterator class; only exposes objects matching a given type.
  template <class T>
  class CastingIterator
  {
    Iterator itsItr;

    void advanceToValid()
    {
      while (!itsItr.at_end() && (dynamic_cast<T*>((*itsItr).getWeak())==0))
        ++itsItr;
    }

  public:
    CastingIterator(const Iterator& begin) : itsItr(begin)
    { advanceToValid(); }

    CastingIterator& operator++() { ++itsItr; advanceToValid(); return *this; }

    bool at_end() const { return itsItr.at_end(); }
    bool is_valid() const { return itsItr.is_valid(); }

    T* operator*() const { return &(dynamic_cast<T&>(**itsItr)); }

    T* operator->() const { return operator*(); }
  };

  //
  // Collection interface
  //

  /// Returns the number of valid objects in the database.
  int count() const throw();

  /// Returns true if 'id' is a valid uid.
  bool isValidId(Nub::UID id) const throw();

  /// Releases the object specified by \a id, but only if it is unshared.
  /** This causes the object to be destroyed since it was unshared. If
      the object is shared, this operation throws an exception. */
  void remove(Nub::UID id);

  /// Removes reference to the object with uid \a id.
  void release(Nub::UID id);

  /// Releases all unshared objects held in the database.
  /** Since the objects are unshared, they will be destroyed in the
      process. */
  void purge();

  /// Calls \c purge() repeatedly until no more items can be removed.
  /** This will get rid of items that were only referenced by other
      items in the list. */
  void clear();

  /// WARNING: should only be called during program exit.
  /** Does a full clear of all objects held by the ObjDb. This breaks
      the usual semantics of ObjDb, since it removes both shared and
      unshared objects. */
  void clearOnExit();

  /// Return the \c Nub::Object* with the uid given by \a id.
  /** Checks first if \a id is a valid uid, and throws an \c
      Nub::InvalidIdError if it is not. */
  Nub::Object* getCheckedObj(Nub::UID id) throw (Nub::InvalidIdError);

  /// Insert a strong reference to obj into the database.
  void insertObj(Nub::Object* obj);

  /// Insert a weak reference to obj into the database.
  void insertObjWeak(Nub::Object* obj);

private:
  ObjDb(const ObjDb&);
  ObjDb& operator=(const ObjDb&);

  Impl* const rep;
};

static const char vcid_groovx_nub_objdb_h_utc20050626084019[] = "$Id$ $HeadURL$";
#endif // !GROOVX_NUB_OBJDB_H_UTC20050626084019_DEFINED
