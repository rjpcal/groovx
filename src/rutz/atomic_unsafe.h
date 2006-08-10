/*!@file rutz/atomic_unsafe.H Thread-UNSAFE integer class */

// //////////////////////////////////////////////////////////////////// //
// The iLab Neuromorphic Vision C++ Toolkit - Copyright (C) 2000-2005   //
// by the University of Southern California (USC) and the iLab at USC.  //
// See http://iLab.usc.edu for information about this project.          //
// //////////////////////////////////////////////////////////////////// //
// Major portions of the iLab Neuromorphic Vision Toolkit are protected //
// under the U.S. patent ``Computation of Intrinsic Perceptual Saliency //
// in Visual Environments, and Applications'' by Christof Koch and      //
// Laurent Itti, California Institute of Technology, 2001 (patent       //
// pending; application number 09/912,225 filed July 23, 2001; see      //
// http://pair.uspto.gov/cgi-bin/final/home.pl for current status).     //
// //////////////////////////////////////////////////////////////////// //
// This file is part of the iLab Neuromorphic Vision C++ Toolkit.       //
//                                                                      //
// The iLab Neuromorphic Vision C++ Toolkit is free software; you can   //
// redistribute it and/or modify it under the terms of the GNU General  //
// Public License as published by the Free Software Foundation; either  //
// version 2 of the License, or (at your option) any later version.     //
//                                                                      //
// The iLab Neuromorphic Vision C++ Toolkit is distributed in the hope  //
// that it will be useful, but WITHOUT ANY WARRANTY; without even the   //
// implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR      //
// PURPOSE.  See the GNU General Public License for more details.       //
//                                                                      //
// You should have received a copy of the GNU General Public License    //
// along with the iLab Neuromorphic Vision C++ Toolkit; if not, write   //
// to the Free Software Foundation, Inc., 59 Temple Place, Suite 330,   //
// Boston, MA 02111-1307 USA.                                           //
// //////////////////////////////////////////////////////////////////// //
//
// Primary maintainer for this file: Rob Peters <rjpeters at usc dot edu>
// $HeadURL$
// $Id$
//

#ifndef RUTZ_ATOMIC_UNSAFE_H_DEFINED
#define RUTZ_ATOMIC_UNSAFE_H_DEFINED

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

  //! Get the current value.
  int atomic_get()
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

#endif // RUTZ_ATOMIC_UNSAFE_H_DEFINED
