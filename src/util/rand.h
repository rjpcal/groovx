///////////////////////////////////////////////////////////////////////
//
// rand.h
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Fri Jun 25 14:09:24 1999
// commit: $Id$
//
// The random number generator classes here are taken from _The C++
// Programming Language_, 3rd ed., by Bjarne Stroustrup.
//
///////////////////////////////////////////////////////////////////////

#ifndef RAND_H_DEFINED
#define RAND_H_DEFINED

namespace Util
{

/// Uniform random distribution
class Urand
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

}

static const char vcid_rand_h[] = "$Header$";
#endif // !RAND_H_DEFINED
