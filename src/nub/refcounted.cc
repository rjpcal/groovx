///////////////////////////////////////////////////////////////////////
//
// refcounted.cc
//
// Copyright (c) 2000-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Sun Oct 22 14:40:28 2000
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

#ifndef REFCOUNTED_CC_DEFINED
#define REFCOUNTED_CC_DEFINED

#include "refcounted.h"

#include "util/error.h"

#include <limits>

#include <typeinfo>

#define NO_PROF
#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER

namespace
{
  const Nub::RefCounts::Count REFCOUNT_MAX =
    std::numeric_limits<Nub::RefCounts::Count>::max();
}

///////////////////////////////////////////////////////////////////////
//
// RefCounts member definitions
//
///////////////////////////////////////////////////////////////////////

void* Nub::RefCounts::operator new(size_t bytes)
{
  return ::operator new(bytes);
}

void Nub::RefCounts::operator delete(void* space, size_t /*bytes*/)
{
  ::operator delete(space);
}

Nub::RefCounts::RefCounts() throw() :
  itsStrong(0),
  itsWeak(0),
  itsOwnerAlive(true),
  itsVolatile(false)
{
DOTRACE("Nub::RefCounts::RefCounts");
}

Nub::RefCounts::~RefCounts() throw()
{
DOTRACE("Nub::RefCounts::~RefCounts");

  if (itsStrong > 0) PANIC("RefCounts object destroyed before strong refcount fell to 0");
  if (itsWeak > 0) PANIC("RefCounts object destroyed before weak refcount fell to 0");
}

void Nub::RefCounts::acquireWeak() throw()
{
DOTRACE("Nub::RefCounts::acquireWeak");

  if (itsWeak == REFCOUNT_MAX) PANIC("weak refcount overflow");

  ++itsWeak;
}

Nub::RefCounts::Count Nub::RefCounts::releaseWeak() throw()
{
DOTRACE("Nub::RefCounts::releaseWeak");

  if (itsWeak == 0) PANIC("weak refcount already 0 in releaseWeak()");

  const Count result = --itsWeak;

  if (itsWeak == 0)
    {
      if (itsStrong > 0) PANIC("weak refcount fell to 0 before strong refcount");
      delete this;
    }

  return result;
}

void Nub::RefCounts::acquireStrong() throw()
{
DOTRACE("Nub::RefCounts::acquireStrong");

  if (itsVolatile) PANIC("attempt to use strong refcount with volatile object");
  if (itsStrong == REFCOUNT_MAX) PANIC("strong refcount overflow");

  ++itsStrong;
}

Nub::RefCounts::Count Nub::RefCounts::releaseStrong() throw()
{
DOTRACE("Nub::RefCounts::releaseStrong");

  if (itsVolatile) PANIC("attempt to use strong refcount with volatile object");
  if (itsStrong == 0) PANIC("strong refcount already 0 in releaseStrong()");
  if (itsWeak == 0) PANIC("weak refcount prematurely fell to 0");

  return --itsStrong;
}

void Nub::RefCounts::releaseStrongNoDelete() throw()
{
DOTRACE("Nub::RefCounts::releaseStrongNoDelete");

  if (itsStrong == 0) PANIC("strong refcount already 0 in releaseStrongNoDelete()");

  --itsStrong;
}

void Nub::RefCounts::debug_dump() const throw()
{
  dbg_eval_nl(0, this);
  dbg_eval_nl(0, itsStrong);
  dbg_eval_nl(0, itsWeak);
  dbg_eval_nl(0, itsOwnerAlive);
}

///////////////////////////////////////////////////////////////////////
//
// RefCounted member definitions
//
///////////////////////////////////////////////////////////////////////

void* Nub::RefCounted::operator new(size_t bytes)
{
DOTRACE("Nub::RefCounted::operator new");
  return ::operator new(bytes);
}

void Nub::RefCounted::operator delete(void* space, size_t /*bytes*/)
{
DOTRACE("Nub::RefCounted::operator delete");
  ::operator delete(space);
}

Nub::RefCounted::RefCounted() :
  itsRefCounts(new Nub::RefCounts)
{
DOTRACE("Nub::RefCounted::RefCounted");
  dbg_print(7, "RefCounted ctor"); dbg_eval_nl(7, this);

  itsRefCounts->acquireWeak();
}

Nub::RefCounted::~RefCounted() throw()
{
DOTRACE("Nub::RefCounted::~RefCounted");
  dbg_print(7, "RefCounted dtor"); dbg_eval_nl(7, this);
  dbg_dump(7, *itsRefCounts);

  // Must guarantee that (strong-count == 0) when the refcounted object is
  // destroyed. Without that guarantee, weak references will be messed up,
  // since they'll think that the object is still alive (i.e. strong
  // refcount > 0) when it actually is already destroyed.
  if (itsRefCounts->itsStrong > 0)
    PANIC("RefCounted object destroyed before strong refcount dropped to 0");

  itsRefCounts->itsOwnerAlive = false;
  itsRefCounts->releaseWeak();
}

void Nub::RefCounted::markAsVolatile() throw()
{
DOTRACE("Nub::RefCounted::markAsVolatile");
  if (itsRefCounts->itsStrong > 0)
    PANIC("can't make volatile object that already has strong refs");

  if (itsRefCounts->itsVolatile)
    PANIC("object already marked as volatile");

  itsRefCounts->itsVolatile = true;
}

void Nub::RefCounted::incrRefCount() const throw()
{
  itsRefCounts->acquireStrong();
}

void Nub::RefCounted::decrRefCount() const throw()
{
  if (itsRefCounts->releaseStrong() == 0)
    {
      dbg_eval_nl(3, typeid(*this).name());
      delete this;
    }
}

void Nub::RefCounted::decrRefCountNoDelete() const throw()
{
  itsRefCounts->releaseStrongNoDelete();
}

bool Nub::RefCounted::isShared() const throw()
{
DOTRACE("Nub::RefCounted::isShared");

  return (itsRefCounts->itsStrong > 1) || isNotShareable();
  // We check isNotShareable() so that volatile objects always appear
  // shared, so that they cannot be removed from the Nub::ObjDb until
  // they become invalid.
}

bool Nub::RefCounted::isUnshared() const throw()
{
DOTRACE("Nub::RefCounted::isUnshared");
  return !isShared();
}

bool Nub::RefCounted::isNotShareable() const throw()
{
DOTRACE("Nub::RefCounted::isNotShareable");
  return itsRefCounts->itsVolatile;
}

Nub::RefCounts* Nub::RefCounted::refCounts() const throw()
{
DOTRACE("Nub::RefCounted::refCounts");
  return itsRefCounts;
}

int Nub::RefCounted::dbg_RefCount() const throw()
{
  return itsRefCounts->itsStrong;
}

int Nub::RefCounted::dbg_WeakRefCount() const throw()
{
  return itsRefCounts->itsWeak;
}

static const char vcid_refcounted_cc[] = "$Id$ $URL$";
#endif // !REFCOUNTED_CC_DEFINED
