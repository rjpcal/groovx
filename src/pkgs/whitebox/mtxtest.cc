/** @file pkgs/whitebox/mtxtest.cc tcl interface package for testing
    class mtx */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2005-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Wed Feb 23 10:32:07 2005
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

#include "pkgs/whitebox/mtxtest.h"

#include "mtx/mtx.h"
#include "mtx/mtxops.h"

#include "tcl/pkg.h"

#include "rutz/unittest.h"

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

namespace
{
  bool equalEps(const mtx& m1, const mtx& m2, double eps)
  {
    if (m1.shape() != m2.shape()) return false;
    auto m1itr = m1.begin();
    auto m1end = m1.end();
    auto m2itr = m2.begin();
    while (m1itr != m1end)
      {
        const double diff = std::abs(*m1itr - *m2itr);
        if (std::abs(diff) > eps)
          {
            int precision = int(-log10(eps));
            dbg_eval_nl(0, precision);
            m1.print_stdout_named("m1", precision);
            m2.print_stdout_named("m2", precision);
            return false;
          }
        ++m1itr; ++m2itr;
      }
    return true;
  }

  void testPrint()
  {
    for (size_t i = 1; i <= 10; ++i)
      {
        const mtx m1 = rand_mtx(i, i);

        const rutz::fstring s = m1.as_string(20);

        const mtx m2 = mtx::from_string(s.c_str());

        TEST_REQUIRE(equalEps(m1, m2, 1e-40));
      }
  }

  void testMtxPlusEqScalar()
  {
    mtx m = mtx::rowmaj_copy_of({1.0, 2.0, 3.0, 4.0}, 1, 4);
    m += 0.5;
    mtx expected = mtx::rowmaj_copy_of({1.5, 2.5, 3.5, 4.5}, 1, 4);
    TEST_REQUIRE(equalEps(m, expected, 0.001));
  }

  void testMtxMinusEqScalar()
  {
    mtx m = mtx::rowmaj_copy_of({1.0, 2.0, 3.0, 4.0}, 1, 4);
    m -= 2.1;
    mtx expected = mtx::rowmaj_copy_of({-1.1, -0.1, 0.9, 1.9}, 1, 4);
    TEST_REQUIRE(equalEps(m, expected, 0.001));
  }

  void testMtxMulEqScalar()
  {
    mtx m = mtx::rowmaj_copy_of({1.0, 2.0, 3.0, 4.0}, 1, 4);
    m *= 1.6;
    mtx expected = mtx::rowmaj_copy_of({1.6, 3.2, 4.8, 6.4}, 1, 4);
    TEST_REQUIRE(equalEps(m, expected, 0.001));
  }

  void testMtxDivEqScalar()
  {
    mtx m = mtx::rowmaj_copy_of({1.0, 2.0, 3.0, 4.0}, 1, 4);
    m /= 2.0;
    mtx expected = mtx::rowmaj_copy_of({0.5, 1.0, 1.5, 2.0}, 1, 4);
    TEST_REQUIRE(equalEps(m, expected, 0.001));
  }

  void testSlicePlusEqScalar()
  {
    mtx m = mtx::rowmaj_copy_of
      ({1.0, 2.0, 3.0,
          4.0, 5.0, 6.0,
          7.0, 8.0, 9.0}, 3, 3);
    m.row(1) += 0.5;
    mtx expected = mtx::rowmaj_copy_of
      ({1.0, 2.0, 3.0,
          4.5, 5.5, 6.5,
          7.0, 8.0, 9.0}, 3, 3);
    TEST_REQUIRE(equalEps(m, expected, 0.001));
  }

  void testSliceMinusEqScalar()
  {
    mtx m = mtx::rowmaj_copy_of
      ({1.0, 2.0, 3.0,
          4.0, 5.0, 6.0,
          7.0, 8.0, 9.0}, 3, 3);
    m.column(2) -= 2.1;
    mtx expected = mtx::rowmaj_copy_of
      ({1.0, 2.0, 0.9,
          4.0, 5.0, 3.9,
          7.0, 8.0, 6.9}, 3, 3);
    TEST_REQUIRE(equalEps(m, expected, 0.001));
  }

  void testSliceMulEqScalar()
  {
    mtx m = mtx::rowmaj_copy_of
      ({1.0, 2.0, 3.0,
          4.0, 5.0, 6.0,
          7.0, 8.0, 9.0}, 3, 3);
    m.row(0) *= 1.6;
    mtx expected = mtx::rowmaj_copy_of
      ({1.6, 3.2, 4.8,
          4.0, 5.0, 6.0,
          7.0, 8.0, 9.0}, 3, 3);
    TEST_REQUIRE(equalEps(m, expected, 0.001));
  }

  void testSliceDivEqScalar()
  {
    mtx m = mtx::rowmaj_copy_of
      ({1.0, 2.0, 3.0,
          4.0, 5.0, 6.0,
          7.0, 8.0, 9.0}, 3, 3);
    m.column(2) /= 2.0;
    mtx expected = mtx::rowmaj_copy_of
      ({1.0, 2.0, 1.5,
          4.0, 5.0, 3.0,
          7.0, 8.0, 4.5}, 3, 3);
    TEST_REQUIRE(equalEps(m, expected, 0.001));
  }

  void testMtxClear()
  {
    mtx m = mtx::rowmaj_copy_of
      ({1.0, 2.0, 3.0,
          4.0, 5.0, 6.0,
          7.0, 8.0, 9.0}, 3, 3);
    m.clear(0.42);
    mtx expected = mtx::rowmaj_copy_of
      ({0.42, 0.42, 0.42,
          0.42, 0.42, 0.42,
          0.42, 0.42, 0.42}, 3, 3);
    TEST_REQUIRE(equalEps(m, expected, 0.001));
  }

  void testMtxSquared()
  {
    mtx m = mtx::rowmaj_copy_of
      ({1.0, 2.0, 3.0,
          4.0, 5.0, 6.0,
          7.0, 8.0, 9.0}, 3, 3);
    m = squared(m);
    mtx expected = mtx::rowmaj_copy_of
      ({1.0, 4.0, 9.0,
          16.0, 25.0, 36.0,
          49.0, 64.0, 81.0}, 3, 3);
    TEST_REQUIRE(equalEps(m, expected, 0.1));
  }

  void testMtxMin()
  {
    mtx m1 = mtx::rowmaj_copy_of
      ({1.0, 2.0, 3.0,
          4.0, 5.0, 6.0,
          7.0, 8.0, 9.0}, 3, 3);
    mtx m2 = mtx::rowmaj_copy_of
      ({9.5, 8.5, 7.5,
          6.5, 5.5, 4.5,
          3.5, 2.5, 1.5}, 3, 3);
    mtx expected = mtx::rowmaj_copy_of
      ({1.0, 2.0, 3.0,
          4.0, 5.0, 4.5,
          3.5, 2.5, 1.5}, 3, 3);
    TEST_REQUIRE(equalEps(min(m1,m2), expected, 0.1));
  }

  void testMtxMax()
  {
    mtx m1 = mtx::rowmaj_copy_of
      ({1.0, 2.0, 3.0,
          4.0, 5.0, 6.0,
          7.0, 8.0, 9.0}, 3, 3);
    mtx m2 = mtx::rowmaj_copy_of
      ({9.5, 8.5, 7.5,
          6.5, 5.5, 4.5,
          3.5, 2.5, 1.5}, 3, 3);
    mtx expected = mtx::rowmaj_copy_of
      ({9.5, 8.5, 7.5,
          6.5, 5.5, 6.0,
          7.0, 8.0, 9.0}, 3, 3);
    TEST_REQUIRE(equalEps(max(m1,m2), expected, 0.1));
  }
}

extern "C"
int Mtxtest_Init(Tcl_Interp* interp)
{
GVX_TRACE("Mtxtest_Init");

  return tcl::pkg::init
    (interp, "Mtxtest", "4.0",
     [](tcl::pkg* pkg) {
      DEF_TEST(pkg, testPrint);
      DEF_TEST(pkg, testMtxPlusEqScalar);
      DEF_TEST(pkg, testMtxMinusEqScalar);
      DEF_TEST(pkg, testMtxMulEqScalar);
      DEF_TEST(pkg, testMtxDivEqScalar);
      DEF_TEST(pkg, testSlicePlusEqScalar);
      DEF_TEST(pkg, testSliceMinusEqScalar);
      DEF_TEST(pkg, testSliceMulEqScalar);
      DEF_TEST(pkg, testSliceDivEqScalar);
      DEF_TEST(pkg, testMtxClear);
      DEF_TEST(pkg, testMtxSquared);
      DEF_TEST(pkg, testMtxMin);
      DEF_TEST(pkg, testMtxMax);
    });
}
