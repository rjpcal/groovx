///////////////////////////////////////////////////////////////////////
//
// geomtest.cc
//
// Copyright (c) 2004-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Thu Oct 28 12:13:08 2004
// commit: $Id$
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

#ifndef GEOMTEST_CC_DEFINED
#define GEOMTEST_CC_DEFINED

#include "geom/txform.h"

#include "pkgs/testutils.h"

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER

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
    txform r;
    for (int i = 0; i < 16; ++i)
      r[i] = t1[i] - t2[i];
    return r;
  }

  void testInvert()
  {
    for (int i = 0; i < 1000; ++i)
      {
        txform T = txform::random();
        txform TI = T.inverted();

        txform M1 = T;  M1.transform(TI); // compute T * inv(T)
        txform M2 = TI; M2.transform(T);  // compute inv(T) * T

        dbg_dump(3, M1);
        dbg_dump(3, M2);

        txform I = txform::identity();

        TEST_REQUIRE_APPROX(sum_square(M1 - I), 0.0, 1e-20);
        TEST_REQUIRE_APPROX(sum_square(M2 - I), 0.0, 1e-20);
      }
  }
}

extern "C"
int Geomtest_Init(Tcl_Interp* interp)
{
DOTRACE("Geomtest_Init");

  PKG_CREATE(interp, "Geomtest", "$Revision$");

  DEF_TEST(pkg, testInvert);

  PKG_RETURN;
}

// Need these to avoid dyld errors on Mac OS X
extern "C" int Geomtest_SafeInit(Tcl_Interp*) { return 1; }

extern "C" int Geomtest_Unload(Tcl_Interp* interp, int /*flags*/)
{
DOTRACE("Geomtest_Unload");
  return Tcl::Pkg::unloadDestroy(interp, "Geomtest");
}

extern "C" int Geomtest_SafeUnload(Tcl_Interp*, int /*flags*/) { return 1; }

static const char vcid_geomtest_cc[] = "$Header$";
#endif // !GEOMTEST_CC_DEFINED
