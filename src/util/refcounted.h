///////////////////////////////////////////////////////////////////////
//
// refcounted.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sun Oct 22 14:40:19 2000
// written: Wed Jun 13 10:42:48 2001
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
  RefCounts();

  friend class Util::RefCounted;

  typedef unsigned short Count;

  void acquireWeak();
  Count releaseWeak();

protected:
  void acquireStrong();
  Count releaseStrong();

public:
  Count strongCount() { return itsStrong; }
  Count weakCount() { return itsWeak; }

  bool isShared() { return itsStrong > 1; }
  bool isUnshared() { return !isShared(); }

private:
  RefCounts(const RefCounts&);
  RefCounts& operator=(const RefCounts&);

  Count itsStrong;
  Count itsWeak;
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

  /** Returns true if the object should not be acquired by any strong
      references, because it's lifespan is volatile (such as objects
      representing on-screen windows that can be dismissed by the
      user). The default implementation provided by RefCounted returns
      false. */
  virtual bool isVolatile() const;

  /// Returns the object's (strong) reference count.
  int refCount() const;

  /// Returns the object's strong+weak reference counts.
  RefCounts* refCounts() const;
};

static const char vcid_refcounted_h[] = "$Header$";
#endif // !REFCOUNTED_H_DEFINED
