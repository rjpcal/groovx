/** @file rutz/unittest.h helper functions for implementing unit tests */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Mon Sep 10 18:57:59 2001
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

#ifndef GROOVX_RUTZ_UNITTEST_H_UTC20050626084020_DEFINED
#define GROOVX_RUTZ_UNITTEST_H_UTC20050626084020_DEFINED

#include "rutz/error.h"

#include "rutz/fileposition.h"
#include "rutz/sfmt.h"

#include <cmath>

#include "rutz/debug.h"
GVX_DBG_REGISTER

namespace rutz
{
  inline void test_require(bool expr,
                           const char* expr_string,
                           const rutz::file_pos& pos)
  {
    dbg_print(3, expr_string); dbg_eval_nl(3, expr);
    if (!expr)
      throw rutz::error(rutz::sfmt("%s:%d:\n\texpected: %s\n",
                                   pos.m_file_name,
                                   pos.m_line_no,
                                   expr_string), pos);
  }

  template <class T, class U>
  inline void test_require_eq(const T& expr1,
                              const U& expr2,
                              const char* expr_string1,
                              const char* expr_string2,
                              const rutz::file_pos& pos)
  {
    dbg_print(3, expr_string1); dbg_eval_nl(3, expr1);
    dbg_print(3, expr_string2); dbg_eval_nl(3, expr2);
    if (!(expr1 == expr2))
      {
        const rutz::fstring msg =
          rutz::sfmt("%s:%d: failed test:\n"
                     "\texpected %s (lhs) == %s (rhs)\n"
                     "\tgot: (lhs) %s == %s\n"
                     "\t     (rhs) %s == %s\n",
                     pos.m_file_name, pos.m_line_no,
                     expr_string1, expr_string2,
                     expr_string1, rutz::sconvert(expr1).c_str(),
                     expr_string2, rutz::sconvert(expr2).c_str());
        throw rutz::error(msg, pos);
      }
  }

  const double APPROX_TOL = 1e-40;

  inline bool approx_eq(double a, double b, double tol = APPROX_TOL)
  {
    return fabs(a-b) < fabs(tol);
  }

  inline void test_require_approx_eq(double expr1,
                                     double expr2,
                                     double tol,
                                     const char* expr_string1,
                                     const char* expr_string2,
                                     const rutz::file_pos& pos)
  {
    dbg_print(3, expr_string1); dbg_eval_nl(3, expr1);
    dbg_print(3, expr_string2); dbg_eval_nl(3, expr2);
    dbg_eval_nl(3, tol);
    if (!approx_eq(expr1, expr2, tol))
      {
        const rutz::fstring msg =
          rutz::sfmt("%s:%d: failed test:\n"
                     "\texpected %s (lhs) ~= %s (rhs)\n"
                     "\tgot: (lhs) %s == %g\n"
                     "\t     (rhs) %s == %g\n"
                     "\tallowable tolerance was: %g\n"
                     "\tactual difference was:   %g\n",
                     pos.m_file_name, pos.m_line_no,
                     expr_string1, expr_string2,
                     expr_string1, expr1,
                     expr_string2, expr2,
                     tol,
                     expr1-expr2);
        throw rutz::error(msg, pos);
      }
  }
}

#define TEST_REQUIRE(expr) \
  rutz::test_require(bool(expr), #expr, SRC_POS)

#define TEST_REQUIRE_EQ(expr1, expr2) \
  rutz::test_require_eq(expr1, expr2, \
                        #expr1, #expr2, SRC_POS)

#define TEST_REQUIRE_APPROX(expr1, expr2, tol) \
  rutz::test_require_approx_eq(expr1, expr2, tol, \
                               #expr1, #expr2, SRC_POS)

#define DEF_TEST(pkg, func) pkg->def(#func, "", &func, SRC_POS)

static const char __attribute__((used)) vcid_groovx_rutz_unittest_h_utc20050626084020[] = "$Id$ $HeadURL$";
#endif // !GROOVX_RUTZ_UNITTEST_H_UTC20050626084020_DEFINED
