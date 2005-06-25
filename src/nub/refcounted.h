///////////////////////////////////////////////////////////////////////
//
// refcounted.h
//
// Copyright (c) 2000-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Sun Oct 22 14:40:19 2000
// commit: $Id$
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

#ifndef REFCOUNTED_H_DEFINED
#define REFCOUNTED_H_DEFINED

#include <cstdlib>

namespace Nub
{
  class RefCounts;
  class RefCounted;
}



///////////////////////////////////////////////////////////////////////
/**
 *
 * Nub::RefCounts manages strong+weak reference counts for
 * Nub::RefCounted objects. Its main purpose is to allow weak
 * ref-counting; a client who wants to do weak ref-counting can get a
 * hold of the Nub::RefCounted object's Nub::RefCounts, and then check
 * whether isOwnerAlive() to know whether the Nub::RefCounted object
 * is still valid. This technique is implemented by Nub::SoftRef. The
 * Nub::RefCounts object will delete itself when both its strong and
 * weak counts go to 0.
 *
 **/
///////////////////////////////////////////////////////////////////////

struct Nub::RefCounts
{
public:
  friend class Nub::RefCounted;

  typedef unsigned short Count;

  void* operator new(size_t bytes);
  void operator delete(void* space, size_t bytes);

  RefCounts() throw();

private:
  /// Private destructor since the object destroys itself eventually in releaseWeak().
  ~RefCounts() throw();

public:

  bool isOwnerAlive() const throw() { return itsOwnerAlive; }

  void acquireWeak() throw();
  Count releaseWeak() throw();

  void debug_dump() const throw();

private:
  RefCounts(const RefCounts&) throw();
  RefCounts& operator=(const RefCounts&) throw();

  void acquireStrong() throw();
  Count releaseStrong() throw();
  void releaseStrongNoDelete() throw();

  Count itsStrong;
  Count itsWeak;
  bool itsOwnerAlive;
  bool itsVolatile;
};



///////////////////////////////////////////////////////////////////////
/**
 *
 * Nub::RefCounted is a reference counting base class that allows both
 * strong and weak reference counting. Nub::RefCounted objects use a
 * Nub::RefCounts object to manage their reference counts, so clients
 * that need to know if a Nub::RefCounted object is still around can
 * check the isOwnerAlive() from its Nub::RefCounts object. Finally,
 * subclasses of Nub::RefCounted can declare themselves volatile (by
 * calling markAsVolatile()) if their lifetime cannot be fully
 * controlled by reference-counting; clients of such volatile objects
 * must use weak reference counts only. No manipulation of the
 * reference count is allowed for volatile objects; only the weak
 * reference count may be used.
 *
 **/
///////////////////////////////////////////////////////////////////////

class Nub::RefCounted
{
private:
  RefCounts* const itsRefCounts;

  // These are disallowed since RefCounted's should always be in
  // one-to-one correspondence with their pointee's.
  RefCounted(const RefCounted& other);
  RefCounted& operator=(const RefCounted& other);

protected:
  /** Class-specific operator new; protected to ensure that clients
      use factory functions. */
  void* operator new(size_t bytes);

  /** Class-specific operator delete; private since deletion should
      only happen in RefCounted::decrRefCount. */
  void operator delete(void* space, size_t bytes);

  /// Default constructor.
  RefCounted();

  /** Virtual destructor is protected, so that we can prevent clients
      from instantiating RefCounted's on the stack and from destroying
      them explicitly. Instead, RefCounted objects will only be
      destroyed by a 'delete this' call inside decrRefCount() if the
      reference count falls to zero or below. Clients are forced to
      create RefCounted objects dynamically using \c new, which is
      what we need in order for 'delete this' to be valid later on. */
  virtual ~RefCounted() throw();

  /// Mark this object as a volatile (unshareable) object.
  void markAsVolatile() throw();

public:
  /// Increment the object's reference count.
  /** This operation (on the strong reference count) is not permitted if
      the object is unshareable. Unshareable objects can only have their
      weak reference counts manipulated. */
  void incrRefCount() const throw();

  /// Decrement the object's reference count.
  /** If this causes the reference count to fall to zero or below, the
      pointee and the pointer will be destroyed by a call to 'delete
      this'. This operation (on the strong reference count) is not
      permitted if the object is unshareable. Unshareable objects can only
      have their weak reference counts manipulated. */
  void decrRefCount() const throw();

  /// Decrement the object's reference count, but don't delete it.
  /** Unlike decrRefCount(), the object will NOT be delete'd if the
      reference count falls to zero. This operation (on the strong
      reference count) is not permitted if the object is
      unshareable. Unshareable objects can only have their weak reference
      counts manipulated. */
  void decrRefCountNoDelete() const throw();

  /// Returns true if no external client has sole ownership of the object.
  /** This may occur if either (1) the reference count is greater than one,
      or (2) the object isNotShareable(), meaning that the object itself is
      the only "owner". */
  bool isShared() const throw();

  /// Returns true if there is a sole external owner of the object.
  /** This occurs if the reference count is one or less and the object is
      shareable. */
  bool isUnshared() const throw();

  /** Returns true if the object is not shareable for any reason. This
      could be because its lifespan is volatile (such as objects
      representing on-screen windows that can be dismissed by the
      user). The default is for objects to be shareable; objects can
      declare themselves as unshareable by calling markAsVolatile(). */
  bool isNotShareable() const throw();

  /// Returns the object's reference count manager.
  RefCounts* refCounts() const throw();


  /// FOR TEST/DEBUG ONLY! Returns the object's (strong) reference count.
  int dbg_RefCount() const throw();

  /// FOR TEST/DEBUG ONLY! Returns the object's weak reference count.
  int dbg_WeakRefCount() const throw();
};

static const char vcid_refcounted_h[] = "$Id$ $URL$";
#endif // !REFCOUNTED_H_DEFINED
