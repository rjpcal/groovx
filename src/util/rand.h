///////////////////////////////////////////////////////////////////////
//
// rand.h
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Fri Jun 25 14:09:24 1999
// written: Wed Mar 19 12:45:37 2003
// $Id$
//
// The random number generator classes here are taken from _The C++
// Programming Language_, 3rd ed., by Bjarne Stroustrup.
//
///////////////////////////////////////////////////////////////////////

#ifndef RAND_H_DEFINED
#define RAND_H_DEFINED

namespace Util
{

/// Uniform random distribution in the interval [0,max]
class Randint
{
private:
  unsigned long randx;

public:
  Randint(long s = 0) : randx(s) {}
  void seed(long s) { randx = s; }

  // magic numbers chosen to use 31 bits of a 32-bit long:

  int abs(int x) { return x&0x7fffffff; }
  static double max() { return 2147483648.0; }
  int draw() { return randx = randx*1103515245 + 12345; }

  double fdraw() { return abs(draw())/max(); }

  double fdrawRange(double min, double max)
  {
    return min + fdraw() * (max-min);
  }

  int operator()() { return abs(draw()); }
};

/// Uniform random distribution in the interval [0:n[
class Urand : public Randint
{
public:
  Urand(long s = 0) : Randint(s) {}

  int operator()(int n) { int r = int(n*fdraw()); return (r==n) ? n-1 : r; }
};

}

static const char vcid_rand_h[] = "$Header$";
#endif // !RAND_H_DEFINED
