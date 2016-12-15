/** @file rutz/staticstack.h STL-style class for push/pop stacks with
    a fixed maximum size */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Wed Oct 13 16:34:11 2004
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

#ifndef GROOVX_RUTZ_STATICSTACK_H_UTC20050626084019_DEFINED
#define GROOVX_RUTZ_STATICSTACK_H_UTC20050626084019_DEFINED

#include "rutz/abort.h"

namespace rutz
{
  /// STL-style class for fixed-size stacks whose size is known at compile time.
  /** Because this class doesn't rely on dynamically-allocated memory,
      it can be both highly efficient and exception-safe. */
  template <typename T, unsigned int N>
  class static_stack
  {
  public:
    static_stack() noexcept : vec(), sz(0) {}

    static_stack(const static_stack& other) noexcept :
      vec(), sz(0)
    {
      *this = other;
    }

    static_stack& operator=(const static_stack& other) noexcept
    {
      sz = other.sz;

      for (unsigned int i = 0; i < sz; ++i)
        {
          vec[i] = other.vec[i];
        }

      return *this;
    }

    unsigned int size() const noexcept { return sz; }

    static unsigned int capacity() noexcept { return N; }

    bool push(T p) noexcept
    {
      if (sz >= N)
        return false;

      vec[sz++] = p;
      return true;
    }

    void pop() noexcept
    {
      if (sz == 0)
        GVX_ABORT("underflow in static_stack::pop");

      --sz;
    }

    T top() const noexcept
    {
      return (sz > 0) ? vec[sz-1] : 0;
    }

    T at(unsigned int i) const noexcept
    {
      return (i < sz) ? vec[i] : 0;
    }

    T operator[](unsigned int i) const noexcept { return at(i); }

    typedef       T*       iterator;
    typedef const T* const_iterator;

    iterator begin() noexcept { return &vec[0]; }
    iterator end()   noexcept { return &vec[0] + sz; }

    const_iterator begin() const noexcept { return &vec[0]; }
    const_iterator end()   const noexcept { return &vec[0] + sz; }

  private:
    T vec[N];
    unsigned int sz;
  };
}

#endif // !GROOVX_RUTZ_STATICSTACK_H_UTC20050626084019_DEFINED
