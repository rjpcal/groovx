///////////////////////////////////////////////////////////////////////
//
// misctcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Nov-98
// written: Wed Jul 11 16:39:17 2001
// $Id$
//
// this file contains the implementations for some simple Tcl functions
// that are basically wrappers for standard C library functions, such
// as rand(), sleep(), and usleep()
//
///////////////////////////////////////////////////////////////////////

#ifndef MISCTCL_C_DEFINED
#define MISCTCL_C_DEFINED

#include "tcl/functor.h"
#include "tcl/tclpkg.h"

#include "util/rand.h"
#include "util/randutils.h"

#include <unistd.h>

///////////////////////////////////////////////////////////////////////
//
// MiscTcl Tcl package declarations
//
///////////////////////////////////////////////////////////////////////

namespace MiscTcl
{
  Util::Randint generator;

  double rand(double min, double max)
  {
    return min + generator.fdraw() * (max-min);
  }

  void srand(int seed)
  {
    generator.seed(seed);
  }

  void usleepr(unsigned int usecs, unsigned int reps)
  {
    for ( ; reps > 0; --reps)
      ::usleep(usecs);
  }
}

extern "C"
int Misc_Init(Tcl_Interp* interp) {
  using namespace MiscTcl;

  Tcl::TclPkg* pkg = new Tcl::TclPkg(interp, "Misc", "$Revision$");

  pkg->addCommand( Tcl::makeCmd(interp, &MiscTcl::rand, "rand", "min max") );
  pkg->addCommand( Tcl::makeCmd(interp, &MiscTcl::srand, "srand", "seed") );

  pkg->addCommand( Tcl::makeCmd(interp, &::sleep, "sleep", "secs") );
  // use the standard library sleep() to sleep a specified # of seconds
  //
  // performance: performance is pretty good, considering that we're on
  // a seconds timescale with this command. It seems to use an extra
  // 9msec more than the specified delay

  pkg->addCommand( Tcl::makeCmd(interp, &::usleep, "usleep", "usecs") );
  // use the standard library usleep() to sleep a specified # of microseconds
  //
  // performance: in a real Tcl script, this command chews up an
  // additional 9000usec more than the specified delay, unless the
  // specified number is < 10000, in which case this command invariably
  // takes ~19000 us (ugh)

  pkg->addCommand( Tcl::makeCmd(interp, &MiscTcl::usleepr, "usleepr", "usecs reps") );
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
