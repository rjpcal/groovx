///////////////////////////////////////////////////////////////////////
//
// signaltest.cc
//
// Copyright (c) 2004-2005
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

#include "nub/signal.h"

#include "pkgs/testutils.h"

#include "tcl/tclpkg.h"

#include "util/trace.h"

namespace
{
  int v0 = 0;
  int v1 = 0;

  void v0_callback0() { ++v0; }
  void v1_callback0() { --v1; }

  void v0_callback1(int i) { v0 += i; }
  void v1_callback1(int i) { v1 -= i; }

  void testSlotAdapterFreeFunc0()
  {
    Nub::Signal0 sig0;

    v0 = 0;
    v1 = 0;
    TEST_REQUIRE_EQ(v0, 0);
    TEST_REQUIRE_EQ(v1, 0);

    sig0.emit();
    TEST_REQUIRE_EQ(v0, 0);
    TEST_REQUIRE_EQ(v1, 0);

    Nub::SoftRef<Nub::Slot0> s0 = sig0.connect(&v0_callback0);
    sig0.emit();
    TEST_REQUIRE_EQ(v0, 1);
    TEST_REQUIRE_EQ(v1, 0);

    Nub::SoftRef<Nub::Slot0> s1 = sig0.connect(&v1_callback0);
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
    Nub::Signal0 sig1;
    Nub::Signal0 sig2;
    Nub::Signal0 sig3;

    sig3.connect(&v0_callback0); // sig3 --> v0_callback0

    v0 = 0;
    TEST_REQUIRE_EQ(v0, 0);

    sig1.emit(); TEST_REQUIRE_EQ(v0, 0);
    sig2.emit(); TEST_REQUIRE_EQ(v0, 0);
    sig3.emit(); TEST_REQUIRE_EQ(v0, 1);

    sig2.connect(sig3.slot()); // sig2 --> sig3 --> v0_callback0

    sig1.emit(); TEST_REQUIRE_EQ(v0, 1);
    sig2.emit(); TEST_REQUIRE_EQ(v0, 2);
    sig3.emit(); TEST_REQUIRE_EQ(v0, 3);

    sig1.connect(sig2.slot()); // sig1 --> sig2 --> sig3 --> v0_callback0

    sig1.emit(); TEST_REQUIRE_EQ(v0, 4);
    sig2.emit(); TEST_REQUIRE_EQ(v0, 5);
    sig3.emit(); TEST_REQUIRE_EQ(v0, 6);

    sig2.disconnect(sig3.slot()); // sig1 --> sig2 XX sig3 --> v0_callback0

    sig1.emit(); TEST_REQUIRE_EQ(v0, 6);
    sig2.emit(); TEST_REQUIRE_EQ(v0, 6);
    sig3.emit(); TEST_REQUIRE_EQ(v0, 7);
  }

  void testCyclicSignalSlotChain()
  {
    // This test is to make sure that we don't let ourselves get into
    // infinite loops even if there are cyclic patterns of connections
    // between signals and slots. This is implemented with
    // Nub::SignalBase::Impl::isItEmitting, which is locked by
    // Nub::SignalBase::Impl::Lock on entry to
    // Nub::SignalBase::doEmit().

    Nub::Signal0 sig1;
    Nub::Signal0 sig2;
    Nub::Signal0 sig3;

    sig3.connect(&v0_callback0); // sig3 --> v0_callback0
    sig2.connect(sig3.slot()); // sig2 --> sig3 --> v0_callback0
    sig1.connect(sig2.slot()); // sig1 --> sig2 --> sig3 --> v0_callback0

    sig3.connect(sig1.slot()); // sig1 --> sig2 --> sig3 --> v0_callback0
                               //  ^                 |
                               //  |                 |
                               //  +-----------------+

    v0 = 0;
    TEST_REQUIRE_EQ(v0, 0);

    sig1.emit(); TEST_REQUIRE_EQ(v0, 1);
    sig2.emit(); TEST_REQUIRE_EQ(v0, 2);
    sig3.emit(); TEST_REQUIRE_EQ(v0, 3);
  }

  void testSlotAdapterFreeFunc1()
  {
    Nub::Signal1<int> sig1;

    v0 = 0;
    v1 = 0;
    TEST_REQUIRE_EQ(v0, 0);
    TEST_REQUIRE_EQ(v1, 0);

    sig1.emit(1);
    TEST_REQUIRE_EQ(v0, 0);
    TEST_REQUIRE_EQ(v1, 0);

    Nub::SoftRef<Nub::Slot1<int> > s0 = sig1.connect(&v0_callback1);
    sig1.emit(2);
    TEST_REQUIRE_EQ(v0, 2);
    TEST_REQUIRE_EQ(v1, 0);

    Nub::SoftRef<Nub::Slot1<int> > s1 = sig1.connect(&v1_callback1);
    sig1.emit(3);
    TEST_REQUIRE_EQ(v0, 5);
    TEST_REQUIRE_EQ(v1, -3);

    sig1.disconnect(s0);
    sig1.emit(4);
    TEST_REQUIRE_EQ(v0, 5);
    TEST_REQUIRE_EQ(v1, -7);

    sig1.disconnect(s1);
    sig1.emit(5);
    TEST_REQUIRE_EQ(v0, 5);
    TEST_REQUIRE_EQ(v1, -7);
  }
}

extern "C"
int Signaltest_Init(Tcl_Interp* interp)
{
DOTRACE("Signaltest_Init");

  PKG_CREATE(interp, "Signaltest", "4.$Revision$");

  DEF_TEST(pkg, testSlotAdapterFreeFunc0);
  DEF_TEST(pkg, testSignalSlotChain);
  DEF_TEST(pkg, testCyclicSignalSlotChain);
  DEF_TEST(pkg, testSlotAdapterFreeFunc1);

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

static const char vcid_signaltest_cc[] = "$Id$ $URL$";
#endif // !SIGNALTEST_CC_DEFINED
