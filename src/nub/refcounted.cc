/** @file nub/refcounted.cc reference-counted base class, allowing
    intrusive smart pointers to be used */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Sun Oct 22 14:40:28 2000
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [https://github.com/rjpcal/groovx]
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

#include "refcounted.h"

#include "rutz/error.h"

#include <limits>

#include <typeinfo>

#ifndef GVX_NO_PROF
#define GVX_NO_PROF
#endif

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

///////////////////////////////////////////////////////////////////////
//
// ref_counts member definitions
//
///////////////////////////////////////////////////////////////////////

void* nub::ref_counts::operator new(size_t bytes)
{
  return ::operator new(bytes);
}

void nub::ref_counts::operator delete(void* space, size_t /*bytes*/)
{
  ::operator delete(space);
}

nub::ref_counts::ref_counts() noexcept :
  m_strong(0),
  m_weak(0),
  m_owner_alive(true),
  m_volatile(false)
{
GVX_TRACE("nub::ref_counts::ref_counts");
}

nub::ref_counts::~ref_counts() noexcept
{
GVX_TRACE("nub::ref_counts::~ref_counts");

  if (m_strong.load() > 0) GVX_PANIC("ref_counts object destroyed before strong refcount fell to 0");
  if (m_weak.load() > 0) GVX_PANIC("ref_counts object destroyed before weak refcount fell to 0");
}

void nub::ref_counts::acquire_weak() noexcept
{
GVX_TRACE("nub::ref_counts::acquire_weak");

  if (++m_weak == std::numeric_limits<int>::max())
    GVX_PANIC("weak refcount overflow");
}

int nub::ref_counts::release_weak() noexcept
{
GVX_TRACE("nub::ref_counts::release_weak");

  const int result = --m_weak;

  if (result < 0) GVX_PANIC("weak refcount already 0 in release_weak()");

  if (result == 0)
    {
      if (m_strong.load() > 0) GVX_PANIC("weak refcount fell to 0 before strong refcount");
      delete this;
    }

  return result;
}

void nub::ref_counts::acquire_strong() noexcept
{
GVX_TRACE("nub::ref_counts::acquire_strong");

  if (m_volatile) GVX_PANIC("attempt to use strong refcount with volatile object");
  if (++m_strong == std::numeric_limits<int>::max())
    GVX_PANIC("strong refcount overflow");
}

int nub::ref_counts::release_strong() noexcept
{
GVX_TRACE("nub::ref_counts::release_strong");

  if (m_volatile) GVX_PANIC("attempt to use strong refcount with volatile object");
  if (m_weak.load() == 0) GVX_PANIC("weak refcount prematurely fell to 0");

  const int result = --m_strong;

  if (result < 0) GVX_PANIC("strong refcount already 0 in release_strong()");

  return result;
}

void nub::ref_counts::release_strong_no_delete() noexcept
{
GVX_TRACE("nub::ref_counts::release_strong_no_delete");

  const int result = --m_strong;

  if (result < 0) GVX_PANIC("strong refcount already 0 in release_strong_no_delete()");
}

void nub::ref_counts::debug_dump() const noexcept
{
  dbg_eval_nl(0, this);
  dbg_eval_nl(0, m_strong.load());
  dbg_eval_nl(0, m_weak.load());
  dbg_eval_nl(0, m_owner_alive);
}

///////////////////////////////////////////////////////////////////////
//
// ref_counted member definitions
//
///////////////////////////////////////////////////////////////////////

void* nub::ref_counted::operator new(size_t bytes)
{
GVX_TRACE("nub::ref_counted::operator new");
  return ::operator new(bytes);
}

void nub::ref_counted::operator delete(void* space, size_t /*bytes*/)
{
GVX_TRACE("nub::ref_counted::operator delete");
  ::operator delete(space);
}

nub::ref_counted::ref_counted() :
  m_ref_counts(new nub::ref_counts)
{
GVX_TRACE("nub::ref_counted::ref_counted");
  dbg_print(7, "ref_counted ctor"); dbg_eval_nl(7, this);

  m_ref_counts->acquire_weak();
}

nub::ref_counted::~ref_counted() noexcept
{
GVX_TRACE("nub::ref_counted::~ref_counted");
  dbg_print(7, "ref_counted dtor"); dbg_eval_nl(7, this);
  dbg_dump(7, *m_ref_counts);

  // Must guarantee that (strong-count == 0) when the refcounted object is
  // destroyed. Without that guarantee, weak references will be messed up,
  // since they'll think that the object is still alive (i.e. strong
  // refcount > 0) when it actually is already destroyed.
  if (m_ref_counts->m_strong.load() > 0)
    GVX_PANIC("ref_counted object destroyed before strong refcount dropped to 0");

  m_ref_counts->m_owner_alive = false;
  m_ref_counts->release_weak();
}

void nub::ref_counted::mark_as_volatile() noexcept
{
GVX_TRACE("nub::ref_counted::mark_as_volatile");
  if (m_ref_counts->m_strong.load() > 0)
    GVX_PANIC("can't make volatile object that already has strong refs");

  if (m_ref_counts->m_volatile)
    GVX_PANIC("object already marked as volatile");

  m_ref_counts->m_volatile = true;
}

void nub::ref_counted::incr_ref_count() const noexcept
{
  m_ref_counts->acquire_strong();
}

void nub::ref_counted::decr_ref_count() const noexcept
{
  if (m_ref_counts->release_strong() == 0)
    {
      dbg_eval_nl(3, typeid(*this).name());
      delete this;
    }
}

void nub::ref_counted::decr_ref_count_no_delete() const noexcept
{
  m_ref_counts->release_strong_no_delete();
}

bool nub::ref_counted::is_shared() const noexcept
{
GVX_TRACE("nub::ref_counted::is_shared");

  return (m_ref_counts->m_strong.load() > 1) || is_not_shareable();
  // We check is_not_shareable() so that volatile objects always appear
  // shared, so that they cannot be removed from the nub::objectdb until
  // they become invalid.
}

bool nub::ref_counted::is_unshared() const noexcept
{
GVX_TRACE("nub::ref_counted::is_unshared");
  return !is_shared();
}

bool nub::ref_counted::is_not_shareable() const noexcept
{
GVX_TRACE("nub::ref_counted::is_not_shareable");
  return m_ref_counts->m_volatile;
}

nub::ref_counts* nub::ref_counted::get_counts() const noexcept
{
GVX_TRACE("nub::ref_counted::get_counts");
  return m_ref_counts;
}

int nub::ref_counted::dbg_ref_count() const noexcept
{
  return m_ref_counts->m_strong.load();
}

int nub::ref_counted::dbg_weak_ref_count() const noexcept
{
  return m_ref_counts->m_weak.load();
}
