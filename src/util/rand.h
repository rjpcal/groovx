///////////////////////////////////////////////////////////////////////
//
// rand.h
//
// Copyright (c) 1999-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Fri Jun 25 14:09:24 1999
// commit: $Id$
//
// The random number generator classes here are taken from _The C++
// Programming Language_, 3rd ed., by Bjarne Stroustrup.
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

#ifndef RAND_H_DEFINED
#define RAND_H_DEFINED

#include <cstdlib> // for rand()

namespace Util
{
  class Urand;

  template <class T>
  inline T randRange(const T& min, const T& max)
  {
    return T( (double(rand()) / (double(RAND_MAX)+1.0)) * (max-min) + min );
  }
}

/// Uniform random distribution
class Util::Urand
{
private:
  unsigned long randx;

  int abs(int x) { return x & 0x7fffffff; }
  static double max() { return 2147483648.0 /* == 0x80000000u */; }

  int idraw() { return randx = randx * 0x41c64e6d + 0x3039; }

public:
  Urand(long s = 0) : randx(s) {}
  void seed(long s) { randx = s; }

  /// Uniform random distribution in the interval [0.0:1.0[
  double fdraw()
  {
    return abs(idraw())/max();
  }

  /// Uniform random distribution in the interval [min:max[
  double fdrawRange(double min, double max)
  {
    return min + fdraw() * (max-min);
  }

  /// Uniform random distribution between true:false
  bool booldraw()
  {
    return fdraw() < 0.5;
  }

  /// Uniform random distribution in the interval [0:n[
  int idraw(int n)
  {
    int r = int(n*fdraw()); return (r==n) ? n-1 : r;
  }

  /// Uniform random distribution in the interval [0:n[
  int operator()(int n) { return idraw(n); }
};

static const char vcid_rand_h[] = "$Header$";
#endif // !RAND_H_DEFINED
