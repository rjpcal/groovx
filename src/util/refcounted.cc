///////////////////////////////////////////////////////////////////////
//
// refcounted.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sun Oct 22 14:40:28 2000
// written: Wed Jun 13 12:25:36 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef REFCOUNTED_CC_DEFINED
#define REFCOUNTED_CC_DEFINED

#include "util/refcounted.h"

#include "util/error.h"

#ifndef GCC_COMPILER
#  include <limits>
#else
#  include <climits>
#  define NO_CPP_LIMITS
#endif

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// RefCounts member definitions
//
///////////////////////////////////////////////////////////////////////

Util::RefCounts::RefCounts() : itsStrong(0), itsWeak(0)
{
DOTRACE("Util::RefCounts::RefCounts");
}

Util::RefCounts::~RefCounts() {
DOTRACE("Util::RefCounts::~RefCounts");

  Assert(itsStrong == 0 && itsWeak == 0);
}

void Util::RefCounts::acquireWeak() {
DOTRACE("Util::RefCounts::acquireWeak");

#ifndef NO_CPP_LIMITS
  Assert(itsWeak < std::numeric_limits<Count>::max());
#else
  Assert(itsWeak < USHRT_MAX);
#endif

  ++itsWeak;
}

Util::RefCounts::Count Util::RefCounts::releaseWeak() {
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

void Util::RefCounts::acquireStrong() {
DOTRACE("Util::RefCounts::acquireStrong");

#ifndef NO_CPP_LIMITS
  Assert(itsStrong < std::numeric_limits<Count>::max());
#else
  Assert(itsStrong < USHRT_MAX);
#endif

  ++itsStrong;
}

Util::RefCounts::Count Util::RefCounts::releaseStrong() {
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


///////////////////////////////////////////////////////////////////////
//
// RefCounted member definitions
//
///////////////////////////////////////////////////////////////////////

void* Util::RefCounted::operator new(size_t bytes) {
DOTRACE("Util::RefCounted::operator new");
  return ::operator new(bytes);
}

void Util::RefCounted::operator delete(void* space, size_t /*bytes*/) {
DOTRACE("Util::RefCounted::operator delete");
  ::operator delete(space);
}

Util::RefCounted::RefCounted() : itsRefCounts(new Util::RefCounts)
{
DOTRACE("Util::RefCounted::RefCounted");
  DebugEval((void*)this);

  itsRefCounts->acquireWeak();
}

Util::RefCounted::~RefCounted()
{
DOTRACE("Util::RefCounted::~RefCounted");

  itsRefCounts->releaseWeak();
}

void Util::RefCounted::incrRefCount() const {
DOTRACE("Util::RefCounted::incrRefCount");
  DebugEvalNL((void*)this);

  itsRefCounts->acquireStrong();
}

void Util::RefCounted::decrRefCount() const {
DOTRACE("Util::RefCounted::decrRefCount");
  DebugEvalNL((void*)this);

  if (itsRefCounts->releaseStrong() == 0)
    delete this;
}

bool Util::RefCounted::isShared() const {
DOTRACE("Util::RefCounted::isShared");
  return itsRefCounts->isShared();
}

bool Util::RefCounted::isUnshared() const {
DOTRACE("Util::RefCounted::isUnshared");
  return itsRefCounts->isUnshared();
}

bool Util::RefCounted::isVolatile() const {
DOTRACE("Util::RefCounted::isVolatile");
  return false;
}

int Util::RefCounted::refCount() const {
DOTRACE("Util::RefCounted::refCount");

  DebugEval(itsRefCounts->weakCount());
  DebugEvalNL(itsRefCounts->strongCount());

  return itsRefCounts->strongCount();
}

Util::RefCounts* Util::RefCounted::refCounts() const {
DOTRACE("Util::RefCounted::refCounts");
  return itsRefCounts;
}

static const char vcid_refcounted_cc[] = "$Header$";
#endif // !REFCOUNTED_CC_DEFINED
