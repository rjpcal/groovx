///////////////////////////////////////////////////////////////////////
//
// refcounted.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sun Oct 22 14:40:19 2000
// written: Tue Jun 12 07:22:39 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef REFCOUNTED_H_DEFINED
#define REFCOUNTED_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(CSTDLIB_DEFINED)
#include <cstdlib>
#define CSTDLIB_DEFINED
#endif

namespace Util
{
  class RefCounts;
  class RefCounted;
}



///////////////////////////////////////////////////////////////////////
/**
 *
 * RefCounts is a class to hold a strong and weak ref count.
 *
 **/
///////////////////////////////////////////////////////////////////////

struct Util::RefCounts {
protected:
  ~RefCounts();

public:
  RefCounts() : strong(0), weak(0) {}

  typedef unsigned short Count;

  void acquireWeak();
  Count releaseWeak();

  void acquireStrong();
  Count releaseStrong();

  Count strongCount() { return strong; }
  Count weakCount() { return weak; }

  bool isShared() { return strong > 1; }
  bool isUnshared() { return !isShared(); }

private:
  RefCounts(const RefCounts&);
  RefCounts& operator=(const RefCounts&);

  Count strong;
  Count weak;
};



///////////////////////////////////////////////////////////////////////
/**
 *
 * RefCounted is a reference counting base class.
 *
 **/
///////////////////////////////////////////////////////////////////////

class Util::RefCounted {
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
  void incrRefCount() const;

  /** Decrement the object's reference count. If this causes the
      reference count to fall to zero or below, the pointee and the
      pointer will be destroyed by a call to 'delete this'. */
  void decrRefCount() const;

  /// Returns true if the reference count is greater than one.
  bool isShared() const;

  /// Returns true if the reference count is one or less.
  bool isUnshared() const;

  /// Returns the object's (strong) reference count.
  RefCounts::Count refCount() const;

  /// Returns the object's strong+weak reference counts.
  RefCounts* refCounts() const;
};

static const char vcid_refcounted_h[] = "$Header$";
#endif // !REFCOUNTED_H_DEFINED
