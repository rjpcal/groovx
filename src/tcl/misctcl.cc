///////////////////////////////////////////////////////////////////////
//
// misctcl.cc
//
// Copyright (c) 1998-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Mon Nov  2 08:00:00 1998
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef MISCTCL_CC_DEFINED
#define MISCTCL_CC_DEFINED

// this file contains the implementations for some simple Tcl functions
// that are basically wrappers for standard C library functions, including
// rand(), sleep(), and usleep()

#include "tcl/tclpkg.h"

#include "tcl/tclcommandgroup.h"

#include "util/error.h"
#include "util/rand.h"
#include "util/strings.h"

#include <unistd.h>

#include "util/trace.h"

namespace
{
  Util::Urand generator;

  void usleepr(unsigned int usecs, unsigned int reps)
  {
    for ( ; reps > 0; --reps)
      ::usleep(usecs);
  }

  fstring backTrace()
  {
    const Util::BackTrace& bt = Util::Error::lastBackTrace();

    fstring result;

    for (unsigned int i = bt.size(); i > 0; --i)
      {
        result.append(bt[i-1]->name(), "\n");
      }

    return result;
  }

  fstring cmdUsage(Tcl::Context& ctx)
  {
    const char* name = ctx.getValFromArg<const char*>(1);
    Tcl::CommandGroup* cmd = Tcl::CommandGroup::lookup(ctx.interp(), name);

    if (cmd == 0)
      throw Util::Error("no such Tcl::CommandGroup");

    return cmd->usage();
  }
}

extern "C"
int Misc_Init(Tcl_Interp* interp)
{
DOTRACE("Misc_Init");

  using namespace Util;

  PKG_CREATE(interp, "Misc", "$Revision$");

  pkg->def( "::rand", "min max",
            bindFirst(memFunc(&Urand::fdrawRange), &generator) );
  pkg->def( "::srand", "seed",
            bindFirst(memFunc(&Urand::seed), &generator) );

  // use the standard library sleep() to sleep a specified # of seconds
  //
  // performance: performance is pretty good, considering that we're on
  // a seconds timescale with this command. It seems to use an extra
  // 9msec more than the specified delay
  pkg->def( "::sleep", "secs", &::sleep );

  // use the standard library usleep() to sleep a specified # of microseconds
  //
  // performance: in a real Tcl script, this command chews up an
  // additional 9000usec more than the specified delay, unless the
  // specified number is < 10000, in which case this command invariably
  // takes ~19000 us (ugh)
  pkg->def( "::usleep", "usecs", &::usleep );

  // use the standard library usleep() to repeatedly sleep a specified #
  // of microseconds
  //
  // performance: as with usleepCmd, there is some significant overhead
  // here. It is typically an extra 10000usec per loop iteration, but
  // again, as in usleepCmd, there seemse to be a minimum of ~20000usec
  // per iteration, even if the specified delay is 1.
  pkg->def( "::usleepr", "usecs reps", &usleepr );

  pkg->def( "::bt", "", &backTrace );

  pkg->defRaw( "::?", 1, "cmd_name", &cmdUsage );

  PKG_RETURN;
}

static const char vcid_misctcl_cc[] = "$Header$";
#endif // !MISCTCL_CC_DEFINED
