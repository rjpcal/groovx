/** @file tcl/tclpkg-misc.cc tcl interface package for miscellaneous
    functions */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1998-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Mon Nov  2 08:00:00 1998
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [https://github.com/rjpcal/groovx]
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

#include "tcl/tclpkg-misc.h"

// this file contains the implementations for some simple Tcl
// functions that are basically wrappers for standard C library
// functions, including rand(), sleep(), and usleep()

#include "tcl/commandgroup.h"
#include "tcl/list.h"
#include "tcl/pkg.h"

#include "rutz/backtrace.h"
#include "rutz/backtraceformat.h"
#include "rutz/error.h"
#include "rutz/fstring.h"
#include "rutz/rand.h"
#include "rutz/sfmt.h"

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
    rutz::backtrace bt;
    rutz::error::get_last_backtrace(bt);
    return rutz::format(bt);
  }

  void cmdUsage(tcl::call_context& ctx)
  {
    const char* name = ctx.get_arg<const char*>(1);
    tcl::command_group* cmd =
      tcl::command_group::lookup_original(ctx.interp(), name);

    if (cmd == nullptr)
      throw rutz::error("no such tcl::command_group", SRC_POS);

    ctx.set_result( rutz::sfmt("%s resolves to %s\n%s",
                               name, cmd->resolved_name().c_str(),
                               cmd->usage().c_str()) );
  }

  unsigned long get_default_seed() { return rutz::default_rand_seed; }
  void set_default_seed(unsigned long x) { rutz::default_rand_seed = x; }

  rutz::fstring tcl_valuetype(const tcl::obj& obj)
  {
    return obj.tcltype_name();
  }

  double rand_draw(double min, double max)
  {
    return generator.fdraw_range(min, max);
  }

  tcl::list rand_draw_n(double min, double max, int n)
  {
    tcl::list result;
    for (int i = 0; i < n; ++i)
      result.append(rand_draw(min, max));
    return result;
  }

  tcl::list test_echo_with_defaults(int a, int b, int c)
  {
    tcl::list result;
    result.append(a);
    result.append(b);
    result.append(c);
    return result;
  }
}

extern "C"
int Misc_Init(Tcl_Interp* interp)
{
GVX_TRACE("Misc_Init");

  return tcl::pkg::init
    (interp, "Misc", "4.0",
     [](tcl::pkg* pkg) {

      pkg->def( "::rand", "min max", &rand_draw, SRC_POS);
      pkg->def( "::rand", "min max ?n=1?", &rand_draw_n, SRC_POS);
      pkg->def( "::srand", "seed",
                [](unsigned long s){generator.seed(s);}, SRC_POS );

      // use the standard library sleep() to sleep a specified # of
      // seconds
      //
      // performance: performance is pretty good, considering that we're on
      // a seconds timescale with this command. It seems to use an extra
      // 9msec more than the specified delay
      pkg->def( "::sleep", "secs", &::sleep, SRC_POS );

      // use the standard library usleep() to sleep a specified # of
      // microseconds
      //
      // performance: in a real Tcl script, this command chews up an
      // additional 9000usec more than the specified delay, unless the
      // specified number is < 10000, in which case this command
      // invariably takes ~19000 us (ugh)
      pkg->def( "::usleep", "usecs", &::usleep, SRC_POS );

      // use the standard library usleep() to repeatedly sleep a
      // specified # of microseconds
      //
      // performance: as with usleepCmd, there is some significant
      // overhead here. It is typically an extra 10000usec per loop
      // iteration, but again, as in usleepCmd, there seemse to be a
      // minimum of ~20000usec per iteration, even if the specified
      // delay is 1.
      pkg->def( "::usleepr", "usecs reps", &usleepr, SRC_POS );

      pkg->def( "::bt", "", &backTrace, SRC_POS );

      pkg->def( "::default_rand_seed", "", &get_default_seed, SRC_POS );
      pkg->def( "::default_rand_seed", "seed", &set_default_seed, SRC_POS );

      pkg->def( "::tcl_valuetype", "value", &tcl_valuetype, SRC_POS );

      pkg->def_raw( "::?", tcl::arg_spec(2), "cmd_name", &cmdUsage, SRC_POS );

      pkg->def( "::test_echo_with_defaults", "a=1 b=2 c=3",
                &test_echo_with_defaults, SRC_POS, 1, 2, 3 );
    });
}
