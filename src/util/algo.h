///////////////////////////////////////////////////////////////////////
//
// algo.h
//
// Copyright (c) 2001-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Sun Jul 22 23:31:48 2001
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

#ifndef ALGO_H_DEFINED
#define ALGO_H_DEFINED

namespace rutz
{
  template <class T>
  inline const T& min(const T& a, const T& b)
  {
    /* return (a < b) ? a : b; */ // This triggers warnings in some compilers :(
    if (a < b) return a; return b;
  }

  template <class T>
  inline const T& max(const T& a, const T& b)
  {
    /* return (a > b) ? a : b; */ // This triggers warnings in some compilers :(
    if (a > b) return a; return b;
  }

  template <class T>
  inline T abs(const T& val)
  {
    return (val < 0) ? -val : val;
  }

  template <class T>
  inline T clamp(const T& val, const T& lower, const T& upper)
  {
    return rutz::max(lower, rutz::min(upper, val));
  }

  template <class T>
  inline void swap2(T& t1, T& t2)
  {
    T t2_copy = t2;
    t2 = t1;
    t1 = t2_copy;
  }
}

static const char vcid_algo_h[] = "$Id$ $URL$";
#endif // !ALGO_H_DEFINED
