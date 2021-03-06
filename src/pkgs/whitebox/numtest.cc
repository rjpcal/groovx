/** @file pkgs/whitebox/numtest.cc tcl interface package for testing
    math special functions from mtx/mathspecial.cc */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2005-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Tue Feb 22 11:24:07 2005
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

#include "pkgs/whitebox/numtest.h"

#include "mtx/mathspecial.h"

#include "tcl/pkg.h"

#include "rutz/unittest.h"

#include "rutz/trace.h"

namespace
{
  void testErfc()
  {
    // Compare our erfc() function with the one from cmath
    TEST_REQUIRE_APPROX(dash::erfc(0), ::erfc(0), 1e-5);
    TEST_REQUIRE_APPROX(dash::erfc(1), ::erfc(1), 1e-5);
    TEST_REQUIRE_APPROX(dash::erfc(-1), ::erfc(-1), 1e-5);

    // Compare our erfc() function with known values
    TEST_REQUIRE_APPROX(dash::erfc(0), 1, 1e-5);
    TEST_REQUIRE_APPROX(dash::erfc(1), 0.1573, 1e-5);
    TEST_REQUIRE_APPROX(dash::erfc(-1), 1.8427, 1e-5);

    // Check the symmetry properties of our erfc function
    //  erfc(x)+erfc(-x) == 2
    TEST_REQUIRE_APPROX(dash::erfc(1), 2-dash::erfc(-1), 1e-5);
    TEST_REQUIRE_APPROX(dash::erfc(2), 2-dash::erfc(-2), 1e-5);


    static rutz::prof p1("testprof/erfc/custom", __FILE__, __LINE__);
    static rutz::prof p2("testprof/erfc/builtin", __FILE__, __LINE__);

    {
      rutz::trace t(p1, false);
      double x = 0.0;
      for (int i = 0; i < 2500; ++i)
        for (int f = 1; f <= 100; ++f)
          x += dash::erfc(double(i)/1000.0);
    }

    {
      rutz::trace t(p2, false);
      double x = 0.0;
      for (int i = 0; i < 2500; ++i)
        for (int f = 1; f <= 100; ++f)
          x += ::erfc(double(i)/1000.0);
    }
  }

  void testGammaln()
  {
    // Compare custom gammaln() with builtin lgamma() function
    for (int f = 1; f <= 100; ++f)
      TEST_REQUIRE_APPROX(dash::gammaln(f), ::lgamma(f), 1e-5);

    static rutz::prof p1("testprof/gammaln/custom", __FILE__, __LINE__);
    static rutz::prof p2("testprof/gammaln/builtin", __FILE__, __LINE__);

    {
      rutz::trace t(p1, false);
      double x = 0.0;
      for (int i = 0; i < 2500; ++i)
        for (int f = 1; f <= 100; ++f)
          x += dash::gammaln(f);
    }

    {
      rutz::trace t(p2, false);
      double x = 0.0;
      for (int i = 0; i < 2500; ++i)
        for (int f = 1; f <= 100; ++f)
          x += ::lgamma(f);
    }
  }
}

extern "C"
int Numtest_Init(Tcl_Interp* interp)
{
GVX_TRACE("Numtest_Init");

  return tcl::pkg::init
    (interp, "Numtest", "4.0",
     [](tcl::pkg* pkg) {
      DEF_TEST(pkg, testErfc);
      DEF_TEST(pkg, testGammaln);
    });
}
