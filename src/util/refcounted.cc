///////////////////////////////////////////////////////////////////////
//
// refcounted.cc
//
// Copyright (c) 2000-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Sun Oct 22 14:40:28 2000
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
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

#include <typeinfo>

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER

namespace
{
#ifdef HAVE_LIMITS
  const Util::RefCounts::Count REFCOUNT_MAX =
    std::numeric_limits<Util::RefCounts::Count>::max();
#else
  const Util::RefCounts::Count REFCOUNT_MAX = USHRT_MAX;
#endif
}

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

Util::RefCounts::RefCounts() throw() :
  itsStrong(0),
  itsWeak(0),
  itsOwnerAlive(true),
  itsVolatile(false)
{
DOTRACE("Util::RefCounts::RefCounts");
}

Util::RefCounts::~RefCounts() throw()
{
DOTRACE("Util::RefCounts::~RefCounts");

  if (itsStrong > 0) Panic("RefCounts object destroyed before strong refcount fell to 0");
  if (itsWeak > 0) Panic("RefCounts object destroyed before weak refcount fell to 0");
}

void Util::RefCounts::acquireWeak() throw()
{
DOTRACE("Util::RefCounts::acquireWeak");

  if (itsWeak == REFCOUNT_MAX) Panic("weak refcount overflow");

  ++itsWeak;
}

Util::RefCounts::Count Util::RefCounts::releaseWeak() throw()
{
DOTRACE("Util::RefCounts::releaseWeak");

  if (itsWeak == 0) Panic("weak refcount already 0 in releaseWeak()");

  const Count result = --itsWeak;

  if (itsWeak == 0)
    {
      if (itsStrong > 0) Panic("weak refcount fell to 0 before strong refcount");
      delete this;
    }

  return result;
}

void Util::RefCounts::acquireStrong() throw()
{
DOTRACE("Util::RefCounts::acquireStrong");

  if (itsVolatile) Panic("attempt to use strong refcount with volatile object");
  if (itsStrong == REFCOUNT_MAX) Panic("strong refcount overflow");

  ++itsStrong;
}

Util::RefCounts::Count Util::RefCounts::releaseStrong() throw()
{
DOTRACE("Util::RefCounts::releaseStrong");

  if (itsVolatile) Panic("attempt to use strong refcount with volatile object");
  if (itsStrong == 0) Panic("strong refcount already 0 in releaseStrong()");
  if (itsWeak == 0) Panic("weak refcount prematurely fell to 0");

  return --itsStrong;
}

void Util::RefCounts::releaseStrongNoDelete() throw()
{
DOTRACE("Util::RefCounts::releaseStrongNoDelete");

  if (itsStrong == 0) Panic("strong refcount already 0 in releaseStrongNoDelete()");

  --itsStrong;
}

void Util::RefCounts::debugDump() const throw()
{
  dbgEvalNL(0, this);
  dbgEvalNL(0, itsStrong);
  dbgEvalNL(0, itsWeak);
  dbgEvalNL(0, itsOwnerAlive);
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

Util::RefCounted::RefCounted() :
  itsRefCounts(new Util::RefCounts)
{
DOTRACE("Util::RefCounted::RefCounted");
  dbgPrint(7, "RefCounted ctor"); dbgEvalNL(7, this);

  itsRefCounts->acquireWeak();
}

Util::RefCounted::~RefCounted() throw()
{
DOTRACE("Util::RefCounted::~RefCounted");
  dbgPrint(7, "RefCounted dtor"); dbgEvalNL(7, this);
  dbgDump(7, *itsRefCounts);

  // Must guarantee that (strong-count == 0) when the refcounted object is
  // destroyed. Without that guarantee, weak references will be messed up,
  // since they'll think that the object is still alive (i.e. strong
  // refcount > 0) when it actually is already destroyed.
  if (itsRefCounts->itsStrong > 0)
    Panic("RefCounted object destroyed before strong refcount dropped to 0");

  itsRefCounts->itsOwnerAlive = false;
  itsRefCounts->releaseWeak();
}

void Util::RefCounted::markAsVolatile() throw()
{
DOTRACE("Util::RefCounted::markAsVolatile");
  if (itsRefCounts->itsStrong > 0)
    Panic("can't make volatile object that already has strong refs");

  if (itsRefCounts->itsVolatile)
    Panic("object already marked as volatile");

  itsRefCounts->itsVolatile = true;
}

void Util::RefCounted::incrRefCount() const throw()
{
  itsRefCounts->acquireStrong();
}

void Util::RefCounted::decrRefCount() const throw()
{
  if (itsRefCounts->releaseStrong() == 0)
    {
      dbgEvalNL(3, typeid(*this).name());
      delete this;
    }
}

void Util::RefCounted::decrRefCountNoDelete() const throw()
{
  itsRefCounts->releaseStrongNoDelete();
}

bool Util::RefCounted::isShared() const throw()
{
DOTRACE("Util::RefCounted::isShared");

  return (itsRefCounts->itsStrong > 1) || isNotShareable();
  // We check isNotShareable() so that volatile objects always appear
  // shared, so that they cannot be removed from the ObjDb until they
  // become invalid.
}

bool Util::RefCounted::isUnshared() const throw()
{
DOTRACE("Util::RefCounted::isUnshared");
  return !isShared();
}

bool Util::RefCounted::isNotShareable() const throw()
{
DOTRACE("Util::RefCounted::isNotShareable");
  return itsRefCounts->itsVolatile;
}

Util::RefCounts* Util::RefCounted::refCounts() const throw()
{
DOTRACE("Util::RefCounted::refCounts");
  return itsRefCounts;
}

int Util::RefCounted::dbg_RefCount() const throw()
{
  return itsRefCounts->itsStrong;
}

static const char vcid_refcounted_cc[] = "$Header$";
#endif // !REFCOUNTED_CC_DEFINED
