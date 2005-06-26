///////////////////////////////////////////////////////////////////////
//
// mtxtest.cc
//
// Copyright (c) 2005-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Wed Feb 23 10:32:07 2005
// commit: $Id$
// $HeadURL$
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

#ifndef GROOVX_PKGS_WHITEBOX_MTXTEST_CC_UTC20050626084022_DEFINED
#define GROOVX_PKGS_WHITEBOX_MTXTEST_CC_UTC20050626084022_DEFINED

#include "pkgs/whitebox/mtxtest.h"

#include "pkgs/mtx/mtx.h"
#include "pkgs/mtx/mtxops.h"

#include "tcl/tclpkg.h"

#include "rutz/unittest.h"

#include "rutz/trace.h"

namespace
{
  void testPrint()
  {
    for (int i = 1; i <= 10; ++i)
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
DOTRACE("Mtxtest_Init");

  PKG_CREATE(interp, "Mtxtest", "4.$Revision$");

  DEF_TEST(pkg, testPrint);

  PKG_RETURN;
}

// Need these to avoid dyld errors on Mac OS X
extern "C" int Mtxtest_SafeInit(Tcl_Interp*) { return 1; }

extern "C" int Mtxtest_Unload(Tcl_Interp* interp, int /*flags*/)
{
DOTRACE("Mtxtest_Unload");
  return Tcl::Pkg::unloadDestroy(interp, "Mtxtest");
}

extern "C" int Mtxtest_SafeUnload(Tcl_Interp*, int /*flags*/) { return 1; }

static const char vcid_groovx_pkgs_whitebox_mtxtest_cc_utc20050626084022[] = "$Id$ $HeadURL$";
#endif // !GROOVX_PKGS_WHITEBOX_MTXTEST_CC_UTC20050626084022_DEFINED
