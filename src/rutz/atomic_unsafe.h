/** @file rutz/atomic_unsafe.h Thread-UNSAFE integer class */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Thu Aug 10 16:56:03 2006
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

#ifndef GROOVX_RUTZ_ATOMIC_UNSAFE_H_UTC20070412044712_DEFINED
#define GROOVX_RUTZ_ATOMIC_UNSAFE_H_UTC20070412044712_DEFINED

#include <limits>

namespace rutz
{

/// Thread-UNSAFE integer class.
class unsafe_atomic_int
{
private:
  int x;

  unsafe_atomic_int(const unsafe_atomic_int&);
  unsafe_atomic_int& operator=(const unsafe_atomic_int&);

public:
  //! Construct with an initial value of 0.
  unsafe_atomic_int() : x(0) {}

  //! Get the maximum representable value
  static int max_value() { return std::numeric_limits<int>::max(); }

  //! Get the current value.
  int atomic_get() const
  { return x; }

  //! Set value to the given value \a v.
  void atomic_set(int v)
  { x = v; }

  //! Add \a v to the value.
  void atomic_add(int i)
  { x += i; }

  //! Subtract \a v from the value.
  void atomic_sub(int i)
  { x -= i; }

  //! Subtract \a v from the value; return true if the new value is zero.
  bool atomic_sub_test_zero(int i)
  { return bool((x -= i) == 0); }

  //! Increment the value by one.
  void atomic_incr()
  { ++x; }

  //! Decrement the value by one.
  void atomic_decr()
  { --x; }

  //! Decrement the value by one; return true if the new value is zero.
  bool atomic_decr_test_zero()
  { return bool(--x == 0); }

  //! Increment the value by one; return true if the new value is zero.
  bool atomic_incr_test_zero()
  { return bool(++x == 0); }

  //! Add \a v to the value and return the new value
  int atomic_add_return(int i)
  { return (x += i); }

  //! Subtract \a v from the value and return the new value
  int atomic_sub_return(int i)
  { return (x -= i); }

  //! Increment the value by one and return the new value.
  int atomic_incr_return()
  { return ++x; }

  //! Decrement the value by one and return the new value.
  int atomic_decr_return()
  { return --x; }
};

} // end namespace rutz

// ######################################################################
/* So things look consistent in everyone's emacs... */
/* Local Variables: */
/* mode: c++ */
/* indent-tabs-mode: nil */
/* End: */

static const char __attribute__((used)) vcid_groovx_rutz_atomic_unsafe_h_utc20070412044712[] = "$Id$ $HeadURL$";
#endif // !GROOVX_RUTZ_ATOMIC_UNSAFE_H_UTC20070412044712DEFINED
