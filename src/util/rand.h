///////////////////////////////////////////////////////////////////////
//
// rand.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Fri Jun 25 14:09:24 1999
// written: Fri Jun 25 14:09:26 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef RAND_H_DEFINED
#define RAND_H_DEFINED

class Randint { // uniform distribution in the interval [0,max]
  unsigned long randx;
public:
  Randint(long s = 0) { randx = s; }
  void seed(long s) { randx = s; }

  // magic numbers chosen to use 31 bits of a 32-bit long:

  int abs(int x) { return x&0x7fffffff; }
  static double max() { return 2147483648.0; }
  int draw() { return randx = randx*1103515245 + 12345; }

  double fdraw() { return abs(draw())/max(); }

  int operator()() { return abs(draw()); }
};

class Urand : public Randint { // uniform distribution in the interval [0:n
  int n;
public:
  Urand(int nn) { n = nn; }

  int operator()() { int r = n*fdraw(); return (r==n) ? n-1 : r; }
};

static const char vcid_rand_h[] = "$Header$";
#endif // !RAND_H_DEFINED
