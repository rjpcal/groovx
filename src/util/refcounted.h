///////////////////////////////////////////////////////////////////////
//
// refcounted.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sun Oct 22 14:40:19 2000
// written: Sat May 19 08:44:03 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef REFCOUNTED_H_DEFINED
#define REFCOUNTED_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(CSTDLIB_DEFINED)
#include <cstdlib>
#define CSTDLIB_DEFINED
#endif


///////////////////////////////////////////////////////////////////////
/**
 *
 * RefCounted is a reference counting base class.
 *
 **/
///////////////////////////////////////////////////////////////////////

class RefCounted {
private:
  int itsRefCount;
  int itsRefCount2;

  // These are disallowed since RefCounted's should always be in
  // one-to-one correspondence with their pointee's.
  RefCounted(const RefCounted& other);
  RefCounted& operator=(const RefCounted& other);

protected:
  /** Class-specific operator new; protected to ensure that clients
      use factory functions. */
  void* operator new(size_t bytes);

#ifndef GCC_COMPILER
protected:
#else
public:
#endif
  /** Class-specific operator delete; private since deletion should
      only happen in RefCounted::decrRefCount. */
  void operator delete(void* space, size_t bytes);

protected:
  /** Virtual destructor is protected, so that we can prevent clients
		from instantiating RefCounted's on the stack and from destroying
		them explicitly. Instead, MasterPtr's will only be destroyed by
		a 'delete this' call inside decrRefCount() if the reference
		count falls to zero or below. Clients are forced to create
		MasterPtr's dynamically using \c new, which is what we need in
		order for 'delete this' to be valid later on. */
  virtual ~RefCounted();

  /// Default constructor.
  RefCounted();

public:
  /// Increment the object's reference count.
  void incrRefCount();

  /** Decrement the object's reference count. If this causes the
      reference count to fall to zero or below, the pointee and the
      pointer will be destroyed by a call to 'delete this'. */
  void decrRefCount();

  /// Increment the second (debugging) reference count.
  void incrRefCount2();
  /// Decrement the second (debugging) reference count.
  void decrRefCount2();

  /// Returns true if the reference count is greater than one.
  bool isShared() const;

  /// Returns true if the reference count is one or less.
  bool isUnshared() const;

  /// Returns the object's (total=normal+debugging) reference count.
  int refCount() const;

  /// Returns the object's normal reference count.
  int refCount1() const;

  /// Returns the object's second (debugging) reference count.
  int refCount2() const;
};

static const char vcid_refcounted_h[] = "$Header$";
#endif // !REFCOUNTED_H_DEFINED
