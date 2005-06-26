///////////////////////////////////////////////////////////////////////
//
// tcltimertest.cc
//
// Copyright (c) 2004-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Tue Oct 12 18:25:51 2004
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

#ifndef TCLTIMERTEST_CC_DEFINED
#define TCLTIMERTEST_CC_DEFINED

#include "pkgs/whitebox/tcltimertest.h"

#include "nub/timer.h"

#include "tcl/tclpkg.h"
#include "tcl/tcltimerscheduler.h"

#include "rutz/sharedptr.h"
#include "rutz/unittest.h"

#include <cstdlib> // for abs()
#include <tcl.h>
#include <unistd.h> // for usleep()

#include "rutz/trace.h"
#include "rutz/debug.h"
DBG_REGISTER

namespace
{
  int v0 = 0;
  int v1 = 0;
  int v2 = 0;
  Nub::Timer* tp1 = 0;

  void v0_callback()
  {
    v0 += 1;
  }

  void v1_callback()
  {
    v1 -= 1;
    ASSERT(tp1 != 0);
    if (v1 < -3) tp1->cancel();
  }

  void v2_callback()
  {
    v2 += 2;
  }

  void testTimer1()
  {
    v0 = 0;
    v1 = 0;
    v2 = 0;

    rutz::shared_ptr<Tcl::TimerScheduler> s
      (rutz::make_shared(new Tcl::TimerScheduler));

    Nub::Timer t0(20, false);
    Nub::Timer t1(5,  true);
    Nub::Timer t2(0,  false);

    tp1 = &t1;

    TEST_REQUIRE(!t0.isPending());
    TEST_REQUIRE(!t1.isPending());
    TEST_REQUIRE(!t2.isPending());

    t0.sigTimeOut.connect(&v0_callback);
    t1.sigTimeOut.connect(&v1_callback);
    t2.sigTimeOut.connect(&v2_callback);

    t0.schedule(s);
    t1.schedule(s);
    t2.schedule(s); // delay==0 ==> callback should happen immediately

    TEST_REQUIRE(t0.isPending());
    TEST_REQUIRE(t1.isPending());
    TEST_REQUIRE(!t2.isPending());

    TEST_REQUIRE_EQ(v2, 2);

    int loops = 0;
    int events = 0;

    while (t0.elapsedMsec() < 500.0 &&
           (t0.isPending() || t1.isPending()))
      {
        if (Tcl_DoOneEvent(TCL_TIMER_EVENTS|TCL_DONT_WAIT) != 0)
          ++events;

        usleep(1000);

        ++loops;
      }

    dbg_eval_nl(3, loops);
    dbg_eval_nl(3, events);

    TEST_REQUIRE_EQ(v0, 1);
    TEST_REQUIRE_EQ(v1, -4);

    tp1 = 0;
  }

  void testTimerCancel()
  {
    v0 = 0;

    rutz::shared_ptr<Tcl::TimerScheduler> s
      (rutz::make_shared(new Tcl::TimerScheduler));

    Nub::Timer t0(10, false);

    t0.sigTimeOut.connect(&v0_callback);

    TEST_REQUIRE(!t0.isPending());

    t0.schedule(s);

    TEST_REQUIRE(t0.isPending());
    TEST_REQUIRE_EQ(v0, 0);

    while (t0.elapsedMsec() < 15.0)
      {
        Tcl_DoOneEvent(TCL_TIMER_EVENTS|TCL_DONT_WAIT);

        t0.cancel();
        usleep(1000);
      }

    TEST_REQUIRE(!t0.isPending());
    TEST_REQUIRE_EQ(v0, 0);
  }

  // Make sure that we don't let ourselves get into an infinite loop
  void testTimerNoInfiniteLoop()
  {
    rutz::shared_ptr<Tcl::TimerScheduler> s
      (rutz::make_shared(new Tcl::TimerScheduler));

    Nub::Timer t(0, true);

    bool caught = false;

    try
      {
        t.schedule(s);
      }
    catch (rutz::error&)
      {
        caught = true;
      }

    TEST_REQUIRE(caught == true);
  }

  // Should test the way that the Tcl::Interp responds if an error
  // occurs during the timer callback.
  void testTimerCallbackError()
  {
  }
}

extern "C"
int Tcltimertest_Init(Tcl_Interp* interp)
{
DOTRACE("Tcltimertest_Init");

  PKG_CREATE(interp, "Tcltimertest", "4.$Revision$");

  DEF_TEST(pkg, testTimer1);
  DEF_TEST(pkg, testTimerCancel);
  DEF_TEST(pkg, testTimerNoInfiniteLoop);
  DEF_TEST(pkg, testTimerCallbackError);

  PKG_RETURN;
}

// Need these to avoid dyld errors on Mac OS X
extern "C" int Tcltimertest_SafeInit(Tcl_Interp*)
{ return 1; }

extern "C" int Tcltimertest_Unload(Tcl_Interp* interp, int /*flags*/)
{
DOTRACE("Tcltimertest_Unload");
  return Tcl::Pkg::unloadDestroy(interp, "Tcltimertest");
}

extern "C" int Tcltimertest_SafeUnload(Tcl_Interp*, int /*flags*/)
{ return 1; }

static const char vcid_tcltimertest_cc[] = "$Id$ $URL$";
#endif // !TCLTIMERTEST_CC_DEFINED
