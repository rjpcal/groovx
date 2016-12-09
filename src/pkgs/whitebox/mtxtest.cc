/** @file pkgs/whitebox/mtxtest.cc tcl interface package for testing
    class mtx */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2005-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Wed Feb 23 10:32:07 2005
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

#include "pkgs/whitebox/mtxtest.h"

#include "mtx/mtx.h"
#include "mtx/mtxops.h"

#include "tcl/pkg.h"

#include "rutz/unittest.h"

#include "rutz/trace.h"

namespace
{
  void testPrint()
  {
    for (size_t i = 1; i <= 10; ++i)
      {
        const mtx m1 = rand_mtx(i, i);

        const rutz::fstring s = m1.as_string();

        const mtx m2 = mtx::from_string(s.c_str());

        TEST_REQUIRE_APPROX(fabs((m2-m1).max()), 0.0, 1e-40);
      }
  }
}

extern "C"
int Mtxtest_Init(Tcl_Interp* interp)
{
GVX_TRACE("Mtxtest_Init");

  return tcl::pkg::init
    (interp, "Mtxtest", "4.$Revision$",
     [](tcl::pkg* pkg) {
      DEF_TEST(pkg, testPrint);
    });
}
