///////////////////////////////////////////////////////////////////////
//
// randutils.h
// Rob Peters
// created: Mon Mar  8 02:35:53 1999
// written: Wed Mar 29 14:07:35 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef RANDUTILS_H_DEFINED
#define RANDUTILS_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(CSTDLIB_DEFINED)
#include <cstdlib>
#define CSTDLIB_DEFINED
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
