///////////////////////////////////////////////////////////////////////
//
// span.h
//
// Copyright (c) 2004-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Mon Oct 25 12:49:03 2004
// commit: $Id$
// $HeadURL$
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

#ifndef GROOVX_GEOM_SPAN_H_UTC20050626084023_DEFINED
#define GROOVX_GEOM_SPAN_H_UTC20050626084023_DEFINED

#include "rutz/algo.h"
#include "rutz/error.h"

#include "rutz/debug.h"

namespace geom
{
  template <class V>
  class span
  {
  private:

  public:
    span()         : lo(),  hi()  {} // init to zero
    span(V l, V h) : lo(l), hi(h)
    {
      if (lo > hi)
        throw rutz::error("invalid span (lo > hi)", SRC_POS);
    }

    /// Explicit conversion constructor.
    template <class U>
    explicit span(const span<U>& other) : lo(other.lo), hi(other.hi) {}

    static span<V> from_any(V v1, V v2)
    {
      return span<V>(rutz::min(v1, v2), rutz::max(v1, v2));
    }

    span<V>& operator=(const span<V>& i)
    {
      const_cast<V&>(lo) = i.lo;
      const_cast<V&>(hi) = i.hi;
      return *this;
    }

    template <class U>
    bool contains(U val) const
    { return val >= lo && val <= hi; }

    V center() const
    { return (lo+hi)/V(2); }

    V width() const
    { return (hi-lo); }

    span<V> incr_width(V w) const
    {
      const V half = V(0.5*w);
      return span<V>(lo-half,
                     hi+(w-half)); // in case V(0.5*h) != 0.5*h (e.g. int math)
    }

    span<V> with_width(V w) const
    {
      return incr_width(w - width());
    }

    span<V> scaled_by(V factor) const
    {
      return with_width(width() * factor);
    }

    span<V> shifted_by(V shift) const
    {
      return span<V>(lo+shift, hi+shift);
    }

    bool is_void() const
    {
      ASSERT(!(lo > hi));
      return (lo == hi);
    }

    span<V> union_with(const span<V>& other) const
    {
      if (other.is_void()) return *this;
      if (this->is_void()) return other;
      return span<V>(rutz::min(this->lo, other.lo),
                     rutz::max(this->hi, other.hi));
    }

    span<V> including(V val) const
    {
      if      (val < lo) return span<V>(val, hi);
      else if (val > hi) return span<V>(lo, val);

      /* else */         return *this;
    }

    const V lo;
    const V hi;
  };
}

static const char vcid_groovx_geom_span_h_utc20050626084023[] = "$Id$ $HeadURL$";
#endif // !GROOVX_GEOM_SPAN_H_UTC20050626084023_DEFINED
