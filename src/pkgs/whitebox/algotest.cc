/** @file pkgs/whitebox/algotest.cc tcl interface package for testing
    functions from rutz/algo.cc */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Wed Oct 13 12:23:33 2004
// commit: $Id$
// $HeadURL$
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

#ifndef GROOVX_PKGS_WHITEBOX_ALGOTEST_CC_UTC20050626084022_DEFINED
#define GROOVX_PKGS_WHITEBOX_ALGOTEST_CC_UTC20050626084022_DEFINED

#include "pkgs/whitebox/algotest.h"

#include "rutz/algo.h"
#include "rutz/unittest.h"

#include "tcl/pkg.h"

#include "rutz/trace.h"

namespace
{
  void testAlgoMinMax()
  {
    TEST_REQUIRE_EQ(rutz::min(false, true), false);
    TEST_REQUIRE_EQ(rutz::max(false, true), true);

    TEST_REQUIRE_EQ(rutz::min(0, 0), 0);
    TEST_REQUIRE_EQ(rutz::min(-12345, 67890), -12345);

    TEST_REQUIRE_EQ(rutz::max(9876, 9876), 9876);
    TEST_REQUIRE_EQ(rutz::max(-67890, 12345), 12345);

    TEST_REQUIRE_EQ(rutz::min(0.0, 0.0), 0.0);
    TEST_REQUIRE_EQ(rutz::min(-1e-32, -1e-33), -1e-32);

    TEST_REQUIRE_EQ(rutz::max(1e16, 1e16), 1e16);
    TEST_REQUIRE_EQ(rutz::max(1e31, 1e32), 1e32);
  }

  void testAlgoAbs()
  {
    TEST_REQUIRE_EQ(rutz::abs(-1), 1);
    TEST_REQUIRE_EQ(rutz::abs(0), 0);
    TEST_REQUIRE_EQ(rutz::abs(1), 1);

    TEST_REQUIRE_EQ(rutz::abs(-1e23), 1e23);
    TEST_REQUIRE_EQ(rutz::abs(0.0), 0.0);
    TEST_REQUIRE_EQ(rutz::abs(1e-3), 1e-3);
  }

  void testAlgoClamp()
  {
    TEST_REQUIRE_EQ(rutz::clamp(-1, 1, 10), 1);
    TEST_REQUIRE_EQ(rutz::clamp( 1, 1, 10), 1);
    TEST_REQUIRE_EQ(rutz::clamp(10, 1, 10), 10);
    TEST_REQUIRE_EQ(rutz::clamp(11, 1, 10), 10);
  }

  void testAlgoSwap2()
  {
    int a = 3;
    int b = 5;
    rutz::swap2(a, b);

    TEST_REQUIRE_EQ(a, 5);
    TEST_REQUIRE_EQ(b, 3);

    int* pa = &a;
    int* pb = &b;

    TEST_REQUIRE_EQ(*pa, 5);
    TEST_REQUIRE_EQ(*pb, 3);

    rutz::swap2(pa, pb);

    TEST_REQUIRE_EQ(*pa, 3);
    TEST_REQUIRE_EQ(*pb, 5);

    TEST_REQUIRE(pa == &b);
    TEST_REQUIRE(pb == &a);
  }
}

extern "C"
int Algotest_Init(Tcl_Interp* interp)
{
GVX_TRACE("Algotest_Init");

  GVX_PKG_CREATE(pkg, interp, "Algotest", "4.$Revision$");

  DEF_TEST(pkg, testAlgoMinMax);
  DEF_TEST(pkg, testAlgoAbs);
  DEF_TEST(pkg, testAlgoClamp);
  DEF_TEST(pkg, testAlgoSwap2);

  GVX_PKG_RETURN(pkg);
}

// Need these to avoid dyld errors on Mac OS X
extern "C" int Algotest_SafeInit(Tcl_Interp*)
{ return 1; }

extern "C" int Algotest_Unload(Tcl_Interp* interp, int /*flags*/)
{
GVX_TRACE("Algotest_Unload");
  return tcl::pkg::destroy_on_unload(interp, "Algotest");
}

extern "C" int Algotest_SafeUnload(Tcl_Interp*, int /*flags*/)
{ return 1; }

static const char __attribute__((used)) vcid_groovx_pkgs_whitebox_algotest_cc_utc20050626084022[] = "$Id$ $HeadURL$";
#endif // !GROOVX_PKGS_WHITEBOX_ALGOTEST_CC_UTC20050626084022_DEFINED
