///////////////////////////////////////////////////////////////////////
// randutils.h
// Rob Peters
// created: Mon Mar  8 02:35:53 1999
// written: Wed Mar 10 11:08:11 1999
///////////////////////////////////////////////////////////////////////

#ifndef RANDUTILS_H_DEFINED
#define RANDUTILS_H_DEFINED

#ifndef CSTDLIB_INCLUDED
#include <cstdlib>
#define CSTDLIB_INCLUDED
#endif

namespace {
  inline int randIntRange(int min, int max) {
	 return int( (double(rand()) / RAND_MAX) * (max-min) + min );
  }

  inline double randDoubleRange(double min, double max) {
	 return ( (double(rand()) / RAND_MAX) * (max-min) + min );
  }
}

#endif // !RANDUTILS_H_DEFINED
