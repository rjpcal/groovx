///////////////////////////////////////////////////////////////////////
//
// signaltest.cc
//
// Copyright (c) 2004-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Wed Oct 13 08:31:39 2004
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

#ifndef SIGNALTEST_CC_DEFINED
#define SIGNALTEST_CC_DEFINED

#include "pkgs/testutils.h"

#include "util/signal.h"

#include "util/trace.h"

namespace
{
  int v0 = 0;
  int v1 = 0;

  void v0_callback() { ++v0; }
  void v1_callback() { --v1; }

  void testSlotAdapterFreeFunc0()
  {
    Util::Signal0 sig0;

    v0 = 0;
    v1 = 0;
    TEST_REQUIRE_EQ(v0, 0);
    TEST_REQUIRE_EQ(v1, 0);

    sig0.emit();
    TEST_REQUIRE_EQ(v0, 0);
    TEST_REQUIRE_EQ(v1, 0);

    Util::SoftRef<Util::Slot0> s0 = sig0.connect(&v0_callback);
    sig0.emit();
    TEST_REQUIRE_EQ(v0, 1);
    TEST_REQUIRE_EQ(v1, 0);

    Util::SoftRef<Util::Slot0> s1 = sig0.connect(&v1_callback);
    sig0.emit();
    TEST_REQUIRE_EQ(v0, 2);
    TEST_REQUIRE_EQ(v1, -1);

    sig0.disconnect(s0);
    sig0.emit();
    TEST_REQUIRE_EQ(v0, 2);
    TEST_REQUIRE_EQ(v1, -2);

    sig0.disconnect(s1);
    sig0.emit();
    TEST_REQUIRE_EQ(v0, 2);
    TEST_REQUIRE_EQ(v1, -2);
  }

  void testSignalSlotChain()
  {
    Util::Signal0 sig1;
    Util::Signal0 sig2;
    Util::Signal0 sig3;

    sig3.connect(&v0_callback); // sig3 --> v0 callback

    v0 = 0;
    TEST_REQUIRE_EQ(v0, 0);

    sig1.emit(); TEST_REQUIRE_EQ(v0, 0);
    sig2.emit(); TEST_REQUIRE_EQ(v0, 0);
    sig3.emit(); TEST_REQUIRE_EQ(v0, 1);

    sig2.connect(sig3.slot()); // sig2 --> sig3 --> v0 callback

    sig1.emit(); TEST_REQUIRE_EQ(v0, 1);
    sig2.emit(); TEST_REQUIRE_EQ(v0, 2);
    sig3.emit(); TEST_REQUIRE_EQ(v0, 3);

    sig1.connect(sig2.slot()); // sig1 --> sig2 --> sig3 --> v0 callback

    sig1.emit(); TEST_REQUIRE_EQ(v0, 4);
    sig2.emit(); TEST_REQUIRE_EQ(v0, 5);
    sig3.emit(); TEST_REQUIRE_EQ(v0, 6);

    sig2.disconnect(sig3.slot()); // sig1 --> sig2 XX sig3 --> v0 callback

    sig1.emit(); TEST_REQUIRE_EQ(v0, 6);
    sig2.emit(); TEST_REQUIRE_EQ(v0, 6);
    sig3.emit(); TEST_REQUIRE_EQ(v0, 8);
  }
}

extern "C"
int Signaltest_Init(Tcl_Interp* interp)
{
DOTRACE("Signaltest_Init");

  PKG_CREATE(interp, "Signaltest", "$Revision$");

  DEF_TEST(pkg, testSlotAdapterFreeFunc0);
  DEF_TEST(pkg, testSignalSlotChain);

  PKG_RETURN;
}

// Need these to avoid dyld errors on Mac OS X
extern "C" int Signaltest_SafeInit(Tcl_Interp*)
{ return 1; }

extern "C" int Signaltest_Unload(Tcl_Interp* interp, int /*flags*/)
{
DOTRACE("Signaltest_Unload");
  return Tcl::Pkg::unloadDestroy(interp, "Signaltest");
}

extern "C" int Signaltest_SafeUnload(Tcl_Interp*, int /*flags*/)
{ return 1; }

static const char vcid_signaltest_cc[] = "$Header$";
#endif // !SIGNALTEST_CC_DEFINED
