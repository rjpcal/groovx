///////////////////////////////////////////////////////////////////////
//
// refcounted.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sun Oct 22 14:40:19 2000
// written: Sun Aug 26 08:36:36 2001
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
 * Util::RefCounts is a class to hold a strong and weak ref *
 * count. This is mainly a helper class for Util::RefCounted. Its
 * purpose for existence is to allow weak ref-counting; a client who
 * wants to do weak ref-counting can get a hold of the
 * Util::RefCounted object's Util::RefCounts, and then check whether
 * the strongCount() is greater than 0 to see if the Util::RefCounted
 * object is still alive. This technique is implemented by
 * Util::SoftRef. The Util::RefCounts object will delete itself when
 * both its strong and weak counts go to 0.
 *
 **/
///////////////////////////////////////////////////////////////////////

struct Util::RefCounts
{
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

private:
  RefCounts(const RefCounts&);
  RefCounts& operator=(const RefCounts&);

  Count itsStrong;
  Count itsWeak;
};



///////////////////////////////////////////////////////////////////////
/**
 *
 * Util::RefCounted is a reference counting base class that allows
 * both strong and weak reference counting. Util::RefCounted objects
 * use a Util::RefCounts object to keep their reference counts, so
 * clients that need to know if a Util::RefCounted object is still
 * around can check the strong count of its Util::RefCounts
 * object. Finally, subclasses of Util::RefCounted can declare
 * themselves volatile (by overriding isVolatile() to return true), in
 * which case it is assumed that their lifetime cannot be fully
 * controlled by reference-counting, so clients of volatile objects
 * should use weak reference counts only. A volatile objects should
 * keep maintain the lone strong reference to itself; then, when some
 * external signal requires the object to be deleted, it can achieve
 * this by releasing the long strong reference.
 *
 **/
///////////////////////////////////////////////////////////////////////

class Util::RefCounted
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
      them explicitly. Instead, RefCounted objects will only be
      destroyed by a 'delete this' call inside decrRefCount() if the
      reference count falls to zero or below. Clients are forced to
      create RefCounted objects dynamically using \c new, which is
      what we need in order for 'delete this' to be valid later on. */
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
      references, because its lifespan is volatile (such as objects
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
