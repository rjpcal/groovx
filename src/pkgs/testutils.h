///////////////////////////////////////////////////////////////////////
//
// testutils.h
//
// Copyright (c) 2001-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Mon Sep 10 18:57:59 2001
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

#ifndef TESTUTILS_H_DEFINED
#define TESTUTILS_H_DEFINED

#include "util/error.h"

#include "tcl/tclpkg.h"

#include <cmath>

namespace
{
  void testRequireImpl(bool expr, const char* exprString,
                       const char* file, int line)
  {
    if (!expr)
      throw Util::Error(fstring(file, "@", line,
                                ": failed test of: ",exprString));
  }

  template <class T, class U>
  void testRequireEqImpl(const T& expr1,
                         const U& expr2,
                         const char* exprString1,
                         const char* exprString2,
                         const char* file, int line)
  {
    if (!(expr1 == expr2))
      {
        fstring msg(file, "@", line, ": failed test:\n");
        msg.append("\texpected ", exprString1, " == ", exprString2, "\n");
        msg.append("\tgot: ", expr1, " != ", expr2);
        throw Util::Error(msg);
      }
  }

  double APPROX_TOL = 1e-40;

  bool approxEq(double a, double b, double tol = APPROX_TOL)
  {
    return fabs(a-b) < tol;
  }

  void testRequireApproxImpl(double expr1,
                             double expr2,
                             double tol,
                             const char* exprString1,
                             const char* exprString2,
                             const char* file, int line)
  {
    if (!approxEq(expr1, expr2, tol))
      {
        fstring msg(file, "@", line, ": failed test:\n");
        msg.append("\texpected ", exprString1, " ~= ", exprString2, "\n");
        msg.append("\tgot: ", expr1, " != ", expr2);
        throw Util::Error(msg);
      }
  }
}

#define TEST_REQUIRE(expr) \
  testRequireImpl(bool(expr), #expr, __FILE__, __LINE__)

#define TEST_REQUIRE_EQ(expr1, expr2) \
  testRequireEqImpl(expr1, expr2, #expr1, #expr2, __FILE__, __LINE__)

#define TEST_REQUIRE_APPROX(expr1, expr2, tol) \
  testRequireApproxImpl(expr1, expr2, tol, #expr1, #expr2, __FILE__, __LINE__)

#define DEF_TEST(pkg, func) pkg->def(#func, "", &func)

static const char vcid_testutils_h[] = "$Header$";
#endif // !TESTUTILS_H_DEFINED
