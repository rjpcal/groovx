///////////////////////////////////////////////////////////////////////
//
// vectwotest.cc
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon May 12 17:57:12 2003
// written: Mon May 12 18:58:34 2003
// $Id$
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

#ifndef VECTWOTEST_CC_DEFINED
#define VECTWOTEST_CC_DEFINED

#include "gx/vec2.h"

#include "pkgs/testutils.h"

#include "util/debug.h"
#include "util/trace.h"

namespace
{
  using namespace Gfx;

  void testAbs()
  {
    Vec2i v(-1, 3);
    TEST_REQUIRE(v.abs() == Vec2i(1, 3));
  }

  void testLength()
  {
    Vec2d v(3.0, 4.0);
    TEST_REQUIRE(v.length() == 5.0);
  }

  void testSetLength()
  {
    Vec2d v(0.6, 0.8);
    v.setLength(5.0);
    TEST_REQUIRE(approxEq(v.length(), 5.0));
    TEST_REQUIRE(v == Vec2d(3.0, 4.0));
  }

  void testSetPolarRad()
  {
    Vec2d v;
    v.setPolarRad(2.0, M_PI / 4.0);
    TEST_REQUIRE_APPROX(v.x(), 1.41421, 1e-5);
    TEST_REQUIRE_APPROX(v.y(), 1.41421, 1e-5);
  }

  void testThetaDeg()
  {
    TEST_REQUIRE_APPROX(Vec2d( 1.0,  0.0).thetaDeg(), 0.0,       1e-20);
    TEST_REQUIRE_APPROX(Vec2d( 1.0,  0.5).thetaDeg(), 26.5651,   1e-4);
    TEST_REQUIRE_APPROX(Vec2d( 1.0,  1.0).thetaDeg(), 45.0,      1e-20);
    TEST_REQUIRE_APPROX(Vec2d( 0.5,  1.0).thetaDeg(), 63.4349,   1e-4);
    TEST_REQUIRE_APPROX(Vec2d( 0.0,  1.0).thetaDeg(), 90.0,      1e-20);
    TEST_REQUIRE_APPROX(Vec2d(-0.5,  1.0).thetaDeg(), 116.5651,  1e-4);
    TEST_REQUIRE_APPROX(Vec2d(-1.0,  1.0).thetaDeg(), 135.0,     1e-20);
    TEST_REQUIRE_APPROX(Vec2d(-1.0,  0.5).thetaDeg(), 153.4349,  1e-4);
    TEST_REQUIRE_APPROX(Vec2d(-1.0,  0.0).thetaDeg(), 180.0,     1e-20);
    TEST_REQUIRE_APPROX(Vec2d(-1.0, -0.5).thetaDeg(), -153.4349, 1e-4);
    TEST_REQUIRE_APPROX(Vec2d(-1.0, -1.0).thetaDeg(), -135.0,    1e-20);
    TEST_REQUIRE_APPROX(Vec2d(-0.5, -1.0).thetaDeg(), -116.5651, 1e-4);
    TEST_REQUIRE_APPROX(Vec2d(-0.0, -1.0).thetaDeg(), -90.0,     1e-20);
    TEST_REQUIRE_APPROX(Vec2d( 0.5, -1.0).thetaDeg(), -63.4349,  1e-4);
    TEST_REQUIRE_APPROX(Vec2d( 1.0, -1.0).thetaDeg(), -45.0,     1e-20);
    TEST_REQUIRE_APPROX(Vec2d( 1.0, -0.5).thetaDeg(), -26.5651,  1e-4);
  }
}

extern "C"
int Vectwotest_Init(Tcl_Interp* interp)
{
DOTRACE("Vectwotest_Init");

  // Package can't be named "Vec2test" because Tcl doesn't like numerals in
  // package names
  Tcl::Pkg* pkg = new Tcl::Pkg(interp, "Vectwotest", "$Revision$");

  DEF_TEST(pkg, testAbs);
  DEF_TEST(pkg, testLength);
  DEF_TEST(pkg, testSetLength);
  DEF_TEST(pkg, testSetPolarRad);
  DEF_TEST(pkg, testThetaDeg);

  return pkg->initStatus();
}

static const char vcid_vectwotest_cc[] = "$Header$";
#endif // !VECTWOTEST_CC_DEFINED
