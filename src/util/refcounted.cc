///////////////////////////////////////////////////////////////////////
//
// refcounted.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sun Oct 22 14:40:28 2000
// written: Mon Nov 25 11:22:10 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef REFCOUNTED_CC_DEFINED
#define REFCOUNTED_CC_DEFINED

#include "util/refcounted.h"

#include "util/error.h"

#ifdef HAVE_LIMITS
#  include <limits>
#else
#  include <climits>
#endif

#include "util/trace.h"
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// RefCounts member definitions
//
///////////////////////////////////////////////////////////////////////

void* Util::RefCounts::operator new(size_t bytes)
{
  return ::operator new(bytes);
}

void Util::RefCounts::operator delete(void* space, size_t /*bytes*/)
{
  ::operator delete(space);
}

Util::RefCounts::RefCounts() : itsStrong(0), itsWeak(0)
{
DOTRACE("Util::RefCounts::RefCounts");
}

Util::RefCounts::~RefCounts()
{
DOTRACE("Util::RefCounts::~RefCounts");

  Assert(itsStrong == 0 && itsWeak == 0);
}

void Util::RefCounts::acquireWeak()
{
DOTRACE("Util::RefCounts::acquireWeak");

#ifdef HAVE_LIMITS
  Assert(itsWeak < std::numeric_limits<Count>::max());
#else
  Assert(itsWeak < USHRT_MAX);
#endif

  ++itsWeak;
}

Util::RefCounts::Count Util::RefCounts::releaseWeak()
{
DOTRACE("Util::RefCounts::releaseWeak");

  Assert(itsWeak > 0);

  --itsWeak;

  if (itsStrong == 0 && itsWeak == 0)
    {
      delete this;
      return 0;
    }

  return itsWeak;
}

void Util::RefCounts::acquireStrong()
{
DOTRACE("Util::RefCounts::acquireStrong");

#ifdef HAVE_LIMITS
  Assert(itsStrong < std::numeric_limits<Count>::max());
#else
  Assert(itsStrong < USHRT_MAX);
#endif

  ++itsStrong;
}

Util::RefCounts::Count Util::RefCounts::releaseStrong()
{
DOTRACE("Util::RefCounts::releaseStrong");

  Assert(itsStrong > 0);

  --itsStrong;

  if (itsStrong == 0 && itsWeak == 0)
    {
      delete this;
      return 0;
    }

  return itsStrong;
}

void Util::RefCounts::releaseStrongNoDelete()
{
DOTRACE("Util::RefCounts::releaseStrongNoDelete");

  Assert(itsStrong > 0);

  --itsStrong;
}

///////////////////////////////////////////////////////////////////////
//
// RefCounted member definitions
//
///////////////////////////////////////////////////////////////////////

void* Util::RefCounted::operator new(size_t bytes)
{
DOTRACE("Util::RefCounted::operator new");
  return ::operator new(bytes);
}

void Util::RefCounted::operator delete(void* space, size_t /*bytes*/)
{
DOTRACE("Util::RefCounted::operator delete");
  ::operator delete(space);
}

Util::RefCounted::RefCounted() : itsRefCounts(new Util::RefCounts)
{
DOTRACE("Util::RefCounted::RefCounted");
  dbgPrint(3, "RefCounted ctor"); dbgEvalNL(3, (void*)this);

  itsRefCounts->acquireWeak();
}

Util::RefCounted::~RefCounted()
{
DOTRACE("Util::RefCounted::~RefCounted");
  dbgPrint(3, "RefCounted dtor");
  dbgEval(3, (void*)this); dbgEvalNL(3, itsRefCounts->strongCount());

  // Must guarantee that (strong-count == 0) when the refcounted object is
  // destroyed. Without that guarantee, weak references will be messed up,
  // since they'll think that the object is still alive (i.e. strong count
  // > 0) when it actually is already destroyed.
  Assert(itsRefCounts->strongCount() == 0);

  itsRefCounts->releaseWeak();
}

void Util::RefCounted::incrRefCount() const
{
  itsRefCounts->acquireStrong();
}

void Util::RefCounted::decrRefCount() const
{
  if (itsRefCounts->releaseStrong() == 0)
    delete this;
}

void Util::RefCounted::decrRefCountNoDelete() const
{
  itsRefCounts->releaseStrongNoDelete();
}

bool Util::RefCounted::isShared() const
{
DOTRACE("Util::RefCounted::isShared");

  return (itsRefCounts->strongCount() > 1) || isNotShareable();
  // We check isNotShareable() so that volatile objects always appear
  // shared, so that they cannot be removed from the ObjDb until they
  // become invalid.
}

bool Util::RefCounted::isUnshared() const
{
DOTRACE("Util::RefCounted::isUnshared");
  return !isShared();
}

bool Util::RefCounted::isNotShareable() const
{
DOTRACE("Util::RefCounted::isNotShareable");
  return false;
}

int Util::RefCounted::refCount() const
{
DOTRACE("Util::RefCounted::refCount");

  dbgEval(3, itsRefCounts->weakCount());
  dbgEvalNL(3, itsRefCounts->strongCount());

  return itsRefCounts->strongCount();
}

Util::RefCounts* Util::RefCounted::refCounts() const
{
DOTRACE("Util::RefCounted::refCounts");
  return itsRefCounts;
}

static const char vcid_refcounted_cc[] = "$Header$";
#endif // !REFCOUNTED_CC_DEFINED
