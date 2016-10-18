/** @file nub/refcounted.h reference-counted base class, allowing
    intrusive smart pointers to be used */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Sun Oct 22 14:40:19 2000
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
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

#ifndef GROOVX_NUB_REFCOUNTED_H_UTC20050626084018_DEFINED
#define GROOVX_NUB_REFCOUNTED_H_UTC20050626084018_DEFINED

#include "rutz/atomic.h"

#include <cstdlib>

namespace nub
{
  class ref_counts;
  class ref_counted;
}


///////////////////////////////////////////////////////////////////////
/**
 *
 * nub::ref_counts manages strong+weak reference counts for
 * nub::ref_counted objects. Its main purpose is to allow weak
 * ref-counting; a client who wants to do weak ref-counting can get a
 * hold of the nub::ref_counted object's nub::ref_counts, and then check
 * whether is_owner_alive() to know whether the nub::ref_counted object
 * is still valid. This technique is implemented by nub::soft_ref. The
 * nub::ref_counts object will delete itself when both its strong and
 * weak counts go to 0.
 *
 **/
///////////////////////////////////////////////////////////////////////

class nub::ref_counts
{
public:
  friend class nub::ref_counted;

  void* operator new(size_t bytes);
  void operator delete(void* space, size_t bytes);

  ref_counts() noexcept;

private:
  /// Private destructor since the object destroys itself eventually in release_weak().
  ~ref_counts() noexcept;

public:

  bool is_owner_alive() const noexcept { return m_owner_alive; }

  void acquire_weak() noexcept;
  int release_weak() noexcept;

  void debug_dump() const noexcept;

private:
  ref_counts(const ref_counts&) noexcept;
  ref_counts& operator=(const ref_counts&) noexcept;

  void acquire_strong() noexcept;
  int release_strong() noexcept;
  void release_strong_no_delete() noexcept;

  rutz::atomic_int_t m_strong;
  rutz::atomic_int_t m_weak;
  bool m_owner_alive;
  bool m_volatile;
};



///////////////////////////////////////////////////////////////////////
/**
 *
 * nub::ref_counted is a reference counting base class that allows both
 * strong and weak reference counting. nub::ref_counted objects use a
 * nub::ref_counts object to manage their reference counts, so clients
 * that need to know if a nub::ref_counted object is still around can
 * check the is_owner_alive() from its nub::ref_counts object. Finally,
 * subclasses of nub::ref_counted can declare themselves volatile (by
 * calling mark_as_volatile()) if their lifetime cannot be fully
 * controlled by reference-counting; clients of such volatile objects
 * must use weak reference counts only. No manipulation of the
 * reference count is allowed for volatile objects; only the weak
 * reference count may be used.
 *
 **/
///////////////////////////////////////////////////////////////////////

class nub::ref_counted
{
private:
  ref_counts* const m_ref_counts;

  // These are disallowed since ref_counted's should always be in
  // one-to-one correspondence with their pointee's.
  ref_counted(const ref_counted& other);
  ref_counted& operator=(const ref_counted& other);

  // If GVX_ENFORCE_FACTORY_FUNCTIONS, then we make operator new() and
  // delete() protected, as well as the default constructor and
  // destructor. This means that only derived classes can use operator
  // new() and delete(), and that thus derived classes are required to
  // define functions of the form "static nub::ref<derived_class>
  // make() { return nub::ref<derived_class>(new derived_class)."
  // While this is a bit of extra typing, it helps ensure that
  // ref_counted objects are not constructed on the stack, and are not
  // handled by the wrong type of smart pointer (such as a
  // shared_ptr).
#ifdef GVX_ENFORCE_FACTORY_FUNCTIONS
protected:
#else
public:
#endif

  /** Class-specific operator new; protected to ensure that clients
      use factory functions. */
  void* operator new(size_t bytes);

  /** Class-specific operator delete; private since deletion should
      only happen in ref_counted::decr_ref_count. */
  void operator delete(void* space, size_t bytes);

  /// Default constructor.
  ref_counted();

  /** Virtual destructor is protected, so that we can prevent clients
      from instantiating ref_counted's on the stack and from destroying
      them explicitly. Instead, ref_counted objects will only be
      destroyed by a 'delete this' call inside decr_ref_count() if the
      reference count falls to zero or below. Clients are forced to
      create ref_counted objects dynamically using \c new, which is
      what we need in order for 'delete this' to be valid later on. */
  virtual ~ref_counted() noexcept;

  /// Mark this object as a volatile (unshareable) object.
  void mark_as_volatile() noexcept;

public:
  /// Increment the object's reference count.
  /** This operation (on the strong reference count) is not permitted if
      the object is unshareable. Unshareable objects can only have their
      weak reference counts manipulated. */
  void incr_ref_count() const noexcept;

  /// Decrement the object's reference count.
  /** If this causes the reference count to fall to zero or below, the
      pointee and the pointer will be destroyed by a call to 'delete
      this'. This operation (on the strong reference count) is not
      permitted if the object is unshareable. Unshareable objects can only
      have their weak reference counts manipulated. */
  void decr_ref_count() const noexcept;

  /// Decrement the object's reference count, but don't delete it.
  /** Unlike decr_ref_count(), the object will NOT be delete'd if the
      reference count falls to zero. This operation (on the strong
      reference count) is not permitted if the object is
      unshareable. Unshareable objects can only have their weak reference
      counts manipulated. */
  void decr_ref_count_no_delete() const noexcept;

  /// Returns true if no external client has sole ownership of the object.
  /** This may occur if either (1) the reference count is greater than one,
      or (2) the object is_not_shareable(), meaning that the object itself is
      the only "owner". */
  bool is_shared() const noexcept;

  /// Returns true if there is a sole external owner of the object.
  /** This occurs if the reference count is one or less and the object is
      shareable. */
  bool is_unshared() const noexcept;

  /** Returns true if the object is not shareable for any reason. This
      could be because its lifespan is volatile (such as objects
      representing on-screen windows that can be dismissed by the
      user). The default is for objects to be shareable; objects can
      declare themselves as unshareable by calling mark_as_volatile(). */
  bool is_not_shareable() const noexcept;

  /// Returns the object's reference count manager.
  ref_counts* get_counts() const noexcept;


  /// FOR TEST/DEBUG ONLY! Returns the object's (strong) reference count.
  int dbg_ref_count() const noexcept;

  /// FOR TEST/DEBUG ONLY! Returns the object's weak reference count.
  int dbg_weak_ref_count() const noexcept;
};

#endif // !GROOVX_NUB_REFCOUNTED_H_UTC20050626084018_DEFINED
