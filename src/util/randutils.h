///////////////////////////////////////////////////////////////////////
// randutils.h
// Rob Peters
// created: Mon Mar  8 02:35:53 1999
// written: Mon Apr  5 16:33:48 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef RANDUTILS_H_DEFINED
#define RANDUTILS_H_DEFINED

#ifndef CSTDLIB_INCLUDED
#include <cstdlib>
#define CSTDLIB_INCLUDED
#endif

namespace {
  inline int randIntRange(int min, int max) {
    return int( (double(rand()) / (double(RAND_MAX)+1.0)) * (max-min) + min );
  }

  inline double randDoubleRange(double min, double max) {
    return ( (double(rand()) / (double(RAND_MAX)+1.0)) * (max-min) + min );
  }
}

static const char vcid_randutils_h[] = "$Header$";
#endif // !RANDUTILS_H_DEFINED
