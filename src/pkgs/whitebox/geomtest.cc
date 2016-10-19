/** @file pkgs/whitebox/geomtest.cc tcl interface package for testing
    class geom::txform and related classes */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Thu Oct 28 12:13:08 2004
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

#include "pkgs/whitebox/geomtest.h"

#include "geom/txform.h"

#include "tcl/pkg.h"

#include "rutz/unittest.h"

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

namespace
{
  using geom::txform;

  double sum_square(const txform& t)
  {
    double result=0.0;
    for (int i = 0; i < 16; ++i)
      result += (t[i] * t[i]);
    return result;
  }

  txform operator-(const txform& t1, const txform& t2)
  {
    txform r = txform::no_init();
    for (int i = 0; i < 16; ++i)
      r[i] = t1[i] - t2[i];
    return r;
  }

  void testInvert()
  {
    const txform I = txform::identity();

    for (int i = 0; i < 1000; ++i)
      {
        const txform T = txform::random();
        const txform TI = T.inverted();

        const txform M1 = T.mtx_mul(TI); // compute T * inv(T)
        const txform M2 = TI.mtx_mul(T); // compute inv(T) * T

        dbg_dump(3, M1);
        dbg_dump(3, M2);

        TEST_REQUIRE_APPROX(sum_square(M1 - I), 0.0, 1e-20);
        TEST_REQUIRE_APPROX(sum_square(M2 - I), 0.0, 1e-20);
      }
  }
}

extern "C"
int Geomtest_Init(Tcl_Interp* interp)
{
GVX_TRACE("Geomtest_Init");

  GVX_PKG_CREATE(pkg, interp, "Geomtest", "4.$Revision$");

  DEF_TEST(pkg, testInvert);

  GVX_PKG_RETURN(pkg);
}
