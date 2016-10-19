/** @file pkgs/whitebox/tcltimertest.cc tcl interface package for
    testing class tcl::timer_scheduler */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Tue Oct 12 18:25:51 2004
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
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

#include "pkgs/whitebox/tcltimertest.h"

#include "nub/timer.h"

#include "tcl/pkg.h"
#include "tcl/timerscheduler.h"

#include "rutz/shared_ptr.h"
#include "rutz/unittest.h"

#include <cstdlib> // for abs()
#include <tcl.h>
#include <unistd.h> // for usleep()

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

namespace
{
  int v0 = 0;
  int v1 = 0;
  int v2 = 0;
  nub::timer* tp1 = 0;

  void v0_callback()
  {
    v0 += 1;
  }

  void v1_callback()
  {
    v1 -= 1;
    GVX_ASSERT(tp1 != 0);
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

    rutz::shared_ptr<tcl::timer_scheduler> s
      (rutz::make_shared(new tcl::timer_scheduler));

    nub::timer t0(20, false);
    nub::timer t1(5,  true);
    nub::timer t2(0,  false);

    tp1 = &t1;

    TEST_REQUIRE(!t0.is_pending());
    TEST_REQUIRE(!t1.is_pending());
    TEST_REQUIRE(!t2.is_pending());

    t0.sig_timeout.connect(&v0_callback);
    t1.sig_timeout.connect(&v1_callback);
    t2.sig_timeout.connect(&v2_callback);

    t0.schedule(s);
    t1.schedule(s);
    t2.schedule(s); // delay==0 ==> callback should happen immediately

    TEST_REQUIRE(t0.is_pending());
    TEST_REQUIRE(t1.is_pending());
    TEST_REQUIRE(!t2.is_pending());

    TEST_REQUIRE_EQ(v2, 2);

    int loops = 0;
    int events = 0;

    while (t0.elapsed_msec() < 500.0 &&
           (t0.is_pending() || t1.is_pending()))
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

    rutz::shared_ptr<tcl::timer_scheduler> s
      (rutz::make_shared(new tcl::timer_scheduler));

    nub::timer t0(10, false);

    t0.sig_timeout.connect(&v0_callback);

    TEST_REQUIRE(!t0.is_pending());

    t0.schedule(s);

    TEST_REQUIRE(t0.is_pending());
    TEST_REQUIRE_EQ(v0, 0);

    while (t0.elapsed_msec() < 15.0)
      {
        Tcl_DoOneEvent(TCL_TIMER_EVENTS|TCL_DONT_WAIT);

        t0.cancel();
        usleep(1000);
      }

    TEST_REQUIRE(!t0.is_pending());
    TEST_REQUIRE_EQ(v0, 0);
  }

  // Make sure that we don't let ourselves get into an infinite loop
  void testTimerNoInfiniteLoop()
  {
    rutz::shared_ptr<tcl::timer_scheduler> s
      (rutz::make_shared(new tcl::timer_scheduler));

    nub::timer t(0, true);

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

  // Should test the way that the tcl::interpreter responds if an
  // error occurs during the timer callback.
  void testTimerCallbackError()
  {
  }
}

extern "C"
int Tcltimertest_Init(Tcl_Interp* interp)
{
GVX_TRACE("Tcltimertest_Init");

  GVX_PKG_CREATE(pkg, interp, "Tcltimertest", "4.$Revision$");

  DEF_TEST(pkg, testTimer1);
  DEF_TEST(pkg, testTimerCancel);
  DEF_TEST(pkg, testTimerNoInfiniteLoop);
  DEF_TEST(pkg, testTimerCallbackError);

  GVX_PKG_RETURN(pkg);
}
