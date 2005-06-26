///////////////////////////////////////////////////////////////////////
//
// misctcl.cc
//
// Copyright (c) 1998-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Mon Nov  2 08:00:00 1998
// commit: $Id$
// $HeadURL$
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

#ifndef GROOVX_TCL_MISCTCL_CC_UTC20050626084017_DEFINED
#define GROOVX_TCL_MISCTCL_CC_UTC20050626084017_DEFINED

#include "tcl/misctcl.h"

// this file contains the implementations for some simple Tcl
// functions that are basically wrappers for standard C library
// functions, including rand(), sleep(), and usleep()

#include "tcl/tclcommandgroup.h"
#include "tcl/tcllistobj.h"
#include "tcl/tclpkg.h"

#include "rutz/backtrace.h"
#include "rutz/error.h"
#include "rutz/fstring.h"
#include "rutz/rand.h"

#include <unistd.h>

#include "rutz/trace.h"

namespace
{
  rutz::urand generator;

  void usleepr(unsigned int usecs, unsigned int reps)
  {
    for ( ; reps > 0; --reps)
      ::usleep(usecs);
  }

  rutz::fstring backTrace()
  {
    const rutz::backtrace& bt = rutz::error::last_backtrace();

    return bt.format();
  }

  rutz::fstring cmdUsage(Tcl::Context& ctx)
  {
    const char* name = ctx.getValFromArg<const char*>(1);
    Tcl::CommandGroup* cmd =
      Tcl::CommandGroup::lookupOriginal(ctx.interp(), name);

    if (cmd == 0)
      throw rutz::error("no such Tcl::CommandGroup", SRC_POS);

    rutz::fstring result(name, " resolves to ", cmd->cmdName(), "\n");
    result.append(cmd->usage());
    return result;
  }

  unsigned long get_default_seed() { return rutz::default_rand_seed; }
  void set_default_seed(unsigned long x) { rutz::default_rand_seed = x; }

  rutz::fstring tcl_valuetype(Tcl::ObjPtr obj)
  {
    return obj.typeName();
  }

  double rand_draw(double min, double max)
  {
    return generator.fdraw_range(min, max);
  }

  Tcl::List rand_draw_n(double min, double max, int n)
  {
    Tcl::List result;
    for (int i = 0; i < n; ++i)
      result.append(rand_draw(min, max));
    return result;
  }
}

extern "C"
int Misc_Init(Tcl_Interp* interp)
{
GVX_TRACE("Misc_Init");

  using namespace rutz;

  GVX_PKG_CREATE(pkg, interp, "Misc", "4.$Revision$");

  pkg->def( "::rand", "min max", &rand_draw, SRC_POS);
  pkg->def( "::rand", "min max ?n=1?", &rand_draw_n, SRC_POS);
  pkg->def( "::srand", "seed",
            bind_first(mem_func(&rutz::urand::seed), &generator),
            SRC_POS );

  // use the standard library sleep() to sleep a specified # of seconds
  //
  // performance: performance is pretty good, considering that we're on
  // a seconds timescale with this command. It seems to use an extra
  // 9msec more than the specified delay
  pkg->def( "::sleep", "secs", &::sleep, SRC_POS );

  // use the standard library usleep() to sleep a specified # of microseconds
  //
  // performance: in a real Tcl script, this command chews up an
  // additional 9000usec more than the specified delay, unless the
  // specified number is < 10000, in which case this command invariably
  // takes ~19000 us (ugh)
  pkg->def( "::usleep", "usecs", &::usleep, SRC_POS );

  // use the standard library usleep() to repeatedly sleep a specified #
  // of microseconds
  //
  // performance: as with usleepCmd, there is some significant overhead
  // here. It is typically an extra 10000usec per loop iteration, but
  // again, as in usleepCmd, there seemse to be a minimum of ~20000usec
  // per iteration, even if the specified delay is 1.
  pkg->def( "::usleepr", "usecs reps", &usleepr, SRC_POS );

  pkg->def( "::bt", "", &backTrace, SRC_POS );

  pkg->def( "::default_rand_seed", "", &get_default_seed, SRC_POS );
  pkg->def( "::default_rand_seed", "seed", &set_default_seed, SRC_POS );

  pkg->def( "::tcl_valuetype", "value", &tcl_valuetype, SRC_POS );

  pkg->defRaw( "::?", 1, "cmd_name", &cmdUsage, SRC_POS );

  GVX_PKG_RETURN(pkg);
}

static const char vcid_groovx_tcl_misctcl_cc_utc20050626084017[] = "$Id$ $HeadURL$";
#endif // !GROOVX_TCL_MISCTCL_CC_UTC20050626084017_DEFINED
