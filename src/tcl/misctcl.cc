///////////////////////////////////////////////////////////////////////
//
// misctcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Nov  2 08:00:00 1998
// written: Sat Sep  8 14:25:51 2001
// $Id$
//
// this file contains the implementations for some simple Tcl functions
// that are basically wrappers for standard C library functions, such
// as rand(), sleep(), and usleep()
//
///////////////////////////////////////////////////////////////////////

#ifndef MISCTCL_C_DEFINED
#define MISCTCL_C_DEFINED

#include "tcl/tclpkg.h"

#include "util/rand.h"

#include <unistd.h>

#include "util/trace.h"

namespace
{
  Util::Randint generator;

  void usleepr(unsigned int usecs, unsigned int reps)
  {
    for ( ; reps > 0; --reps)
      ::usleep(usecs);
  }
}

extern "C"
int Misc_Init(Tcl_Interp* interp)
{
DOTRACE("Misc_Init");

  using namespace Util;

  Tcl::Pkg* pkg = new Tcl::Pkg(interp, "Misc", "$Revision$");

  pkg->def( "::rand", "min max",
            bindFirst(memFunc(&Randint::fdrawRange), &generator) );
  pkg->def( "::srand", "seed",
            bindFirst(memFunc(&Randint::seed), &generator) );

  pkg->def( "::sleep", "secs", &::sleep );
  // use the standard library sleep() to sleep a specified # of seconds
  //
  // performance: performance is pretty good, considering that we're on
  // a seconds timescale with this command. It seems to use an extra
  // 9msec more than the specified delay

  pkg->def( "::usleep", "usecs", &::usleep );
  // use the standard library usleep() to sleep a specified # of microseconds
  //
  // performance: in a real Tcl script, this command chews up an
  // additional 9000usec more than the specified delay, unless the
  // specified number is < 10000, in which case this command invariably
  // takes ~19000 us (ugh)

  pkg->def( "::usleepr", "usecs reps", &usleepr );
  // use the standard library usleep() to repeatedly sleep a specified #
  // of microseconds
  //
  // performance: as with usleepCmd, there is some significant overhead
  // here. It is typically an extra 10000usec per loop iteration, but
  // again, as in usleepCmd, there seemse to be a minimum of ~20000usec
  // per iteration, even if the specified delay is 1.

  return pkg->initStatus();
}

static const char vcid_misctcl_cc[] = "$Header$";
#endif // !MISCTCL_C_DEFINED
