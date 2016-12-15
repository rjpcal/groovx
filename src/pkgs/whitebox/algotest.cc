/** @file pkgs/whitebox/algotest.cc tcl interface package for testing
    functions from rutz/algo.cc */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Wed Oct 13 12:23:33 2004
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

#include "pkgs/whitebox/algotest.h"

#include "rutz/algo.h"
#include "rutz/unittest.h"

#include "tcl/pkg.h"

#include <algorithm>
#include <cmath>
#include <utility>

#include "rutz/trace.h"

namespace
{
  void testAlgoMinMax()
  {
    TEST_REQUIRE_EQ(std::min(false, true), false);
    TEST_REQUIRE_EQ(std::max(false, true), true);

    TEST_REQUIRE_EQ(std::min(0, 0), 0);
    TEST_REQUIRE_EQ(std::min(-12345, 67890), -12345);

    TEST_REQUIRE_EQ(std::max(9876, 9876), 9876);
    TEST_REQUIRE_EQ(std::max(-67890, 12345), 12345);

    TEST_REQUIRE_EQ(std::min(0.0, 0.0), 0.0);
    TEST_REQUIRE_EQ(std::min(-1e-32, -1e-33), -1e-32);

    TEST_REQUIRE_EQ(std::max(1e16, 1e16), 1e16);
    TEST_REQUIRE_EQ(std::max(1e31, 1e32), 1e32);
  }

  void testAlgoAbs()
  {
    TEST_REQUIRE_EQ(std::abs(-1), 1);
    TEST_REQUIRE_EQ(std::abs(0), 0);
    TEST_REQUIRE_EQ(std::abs(1), 1);

    TEST_REQUIRE_EQ(std::abs(-1e23), 1e23);
    TEST_REQUIRE_EQ(std::abs(0.0), 0.0);
    TEST_REQUIRE_EQ(std::abs(1e-3), 1e-3);
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
    std::swap(a, b);

    TEST_REQUIRE_EQ(a, 5);
    TEST_REQUIRE_EQ(b, 3);

    int* pa = &a;
    int* pb = &b;

    TEST_REQUIRE_EQ(*pa, 5);
    TEST_REQUIRE_EQ(*pb, 3);

    std::swap(pa, pb);

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

  return tcl::pkg::init
    (interp, "Algotest", "4.0",
     [](tcl::pkg* pkg) {

      DEF_TEST(pkg, testAlgoMinMax);
      DEF_TEST(pkg, testAlgoAbs);
      DEF_TEST(pkg, testAlgoClamp);
      DEF_TEST(pkg, testAlgoSwap2);
    });
}
