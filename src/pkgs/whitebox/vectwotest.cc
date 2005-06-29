///////////////////////////////////////////////////////////////////////
//
// vectwotest.cc
//
// Copyright (c) 2002-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Mon May 12 17:57:12 2003
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

#ifndef GROOVX_PKGS_WHITEBOX_VECTWOTEST_CC_UTC20050626084022_DEFINED
#define GROOVX_PKGS_WHITEBOX_VECTWOTEST_CC_UTC20050626084022_DEFINED

#include "pkgs/whitebox/vectwotest.h"

#include "geom/vec2.h"

#include "tcl/pkg.h"

#include "rutz/unittest.h"

#include "rutz/debug.h"
GVX_DBG_REGISTER
#include "rutz/trace.h"

namespace
{
  using namespace geom;

  void testAbs()
  {
    vec2i v(-1, 3);
    TEST_REQUIRE(v.abs() == vec2i(1, 3));
  }

  void testLength()
  {
    vec2d v(3.0, 4.0);
    TEST_REQUIRE(v.length() == 5.0);
  }

  void testSetLength()
  {
    vec2d v(0.6, 0.8);
    v.set_length(5.0);
    TEST_REQUIRE_APPROX(v.length(), 5.0, 1e-40);
    TEST_REQUIRE(v == vec2d(3.0, 4.0));
  }

  void testSetPolarRad()
  {
    vec2d v;
    v.set_polar_rad(2.0, M_PI / 4.0);
    TEST_REQUIRE_APPROX(v.x(), 1.41421, 1e-5);
    TEST_REQUIRE_APPROX(v.y(), 1.41421, 1e-5);
  }

  void testThetaDeg()
  {
    TEST_REQUIRE_APPROX(vec2d( 1.0,  0.0).theta_deg(), 0.0,       1e-20);
    TEST_REQUIRE_APPROX(vec2d( 1.0,  0.5).theta_deg(), 26.5651,   1e-4);
    TEST_REQUIRE_APPROX(vec2d( 1.0,  1.0).theta_deg(), 45.0,      1e-20);
    TEST_REQUIRE_APPROX(vec2d( 0.5,  1.0).theta_deg(), 63.4349,   1e-4);
    TEST_REQUIRE_APPROX(vec2d( 0.0,  1.0).theta_deg(), 90.0,      1e-20);
    TEST_REQUIRE_APPROX(vec2d(-0.5,  1.0).theta_deg(), 116.5651,  1e-4);
    TEST_REQUIRE_APPROX(vec2d(-1.0,  1.0).theta_deg(), 135.0,     1e-20);
    TEST_REQUIRE_APPROX(vec2d(-1.0,  0.5).theta_deg(), 153.4349,  1e-4);
    TEST_REQUIRE_APPROX(vec2d(-1.0,  0.0).theta_deg(), 180.0,     1e-20);
    TEST_REQUIRE_APPROX(vec2d(-1.0, -0.5).theta_deg(), -153.4349, 1e-4);
    TEST_REQUIRE_APPROX(vec2d(-1.0, -1.0).theta_deg(), -135.0,    1e-20);
    TEST_REQUIRE_APPROX(vec2d(-0.5, -1.0).theta_deg(), -116.5651, 1e-4);
    TEST_REQUIRE_APPROX(vec2d(-0.0, -1.0).theta_deg(), -90.0,     1e-20);
    TEST_REQUIRE_APPROX(vec2d( 0.5, -1.0).theta_deg(), -63.4349,  1e-4);
    TEST_REQUIRE_APPROX(vec2d( 1.0, -1.0).theta_deg(), -45.0,     1e-20);
    TEST_REQUIRE_APPROX(vec2d( 1.0, -0.5).theta_deg(), -26.5651,  1e-4);
  }

  void testSetThetaDeg()
  {
    { vec2d v(1, 0); v.set_theta_deg(0.0);    TEST_REQUIRE_APPROX(v.x(),  1.000000, 1e-4);   TEST_REQUIRE_APPROX(v.y(),  0.000000, 1e-4); }
    { vec2d v(1, 0); v.set_theta_deg(22.5);   TEST_REQUIRE_APPROX(v.x(),  0.923880, 1e-4);   TEST_REQUIRE_APPROX(v.y(),  0.382683, 1e-4); }
    { vec2d v(1, 0); v.set_theta_deg(45.0);   TEST_REQUIRE_APPROX(v.x(),  0.707107, 1e-4);   TEST_REQUIRE_APPROX(v.y(),  0.707107, 1e-4); }
    { vec2d v(1, 0); v.set_theta_deg(67.5);   TEST_REQUIRE_APPROX(v.x(),  0.382683, 1e-4);   TEST_REQUIRE_APPROX(v.y(),  0.923880, 1e-4); }
    { vec2d v(1, 0); v.set_theta_deg(90.0);   TEST_REQUIRE_APPROX(v.x(),  0.000000, 1e-4);   TEST_REQUIRE_APPROX(v.y(),  1.000000, 1e-4); }
    { vec2d v(1, 0); v.set_theta_deg(112.5);  TEST_REQUIRE_APPROX(v.x(), -0.382683, 1e-4);   TEST_REQUIRE_APPROX(v.y(),  0.923880, 1e-4); }
    { vec2d v(1, 0); v.set_theta_deg(135.0);  TEST_REQUIRE_APPROX(v.x(), -0.707107, 1e-4);   TEST_REQUIRE_APPROX(v.y(),  0.707107, 1e-4); }
    { vec2d v(1, 0); v.set_theta_deg(157.5);  TEST_REQUIRE_APPROX(v.x(), -0.923880, 1e-4);   TEST_REQUIRE_APPROX(v.y(),  0.382683, 1e-4); }
    { vec2d v(1, 0); v.set_theta_deg(180.0);  TEST_REQUIRE_APPROX(v.x(), -1.000000, 1e-4);   TEST_REQUIRE_APPROX(v.y(),  0.000000, 1e-4); }
    { vec2d v(1, 0); v.set_theta_deg(202.5);  TEST_REQUIRE_APPROX(v.x(), -0.923880, 1e-4);   TEST_REQUIRE_APPROX(v.y(), -0.382683, 1e-4); }
    { vec2d v(1, 0); v.set_theta_deg(225.0);  TEST_REQUIRE_APPROX(v.x(), -0.707107, 1e-4);   TEST_REQUIRE_APPROX(v.y(), -0.707107, 1e-4); }
    { vec2d v(1, 0); v.set_theta_deg(247.5);  TEST_REQUIRE_APPROX(v.x(), -0.382683, 1e-4);   TEST_REQUIRE_APPROX(v.y(), -0.923880, 1e-4); }
    { vec2d v(1, 0); v.set_theta_deg(270.0);  TEST_REQUIRE_APPROX(v.x(),  0.000000, 1e-4);   TEST_REQUIRE_APPROX(v.y(), -1.000000, 1e-4); }
    { vec2d v(1, 0); v.set_theta_deg(292.5);  TEST_REQUIRE_APPROX(v.x(),  0.382683, 1e-4);   TEST_REQUIRE_APPROX(v.y(), -0.923880, 1e-4); }
    { vec2d v(1, 0); v.set_theta_deg(315.0);  TEST_REQUIRE_APPROX(v.x(),  0.707107, 1e-4);   TEST_REQUIRE_APPROX(v.y(), -0.707107, 1e-4); }
    { vec2d v(1, 0); v.set_theta_deg(337.5);  TEST_REQUIRE_APPROX(v.x(),  0.923880, 1e-4);   TEST_REQUIRE_APPROX(v.y(), -0.382683, 1e-4); }
  }
}

extern "C"
int Vectwotest_Init(Tcl_Interp* interp)
{
GVX_TRACE("Vectwotest_Init");

  // Package can't be named "vec2test" because Tcl doesn't like numerals in
  // package names
  GVX_PKG_CREATE(pkg, interp, "Vectwotest", "4.$Revision$");

  DEF_TEST(pkg, testAbs);
  DEF_TEST(pkg, testLength);
  DEF_TEST(pkg, testSetLength);
  DEF_TEST(pkg, testSetPolarRad);
  DEF_TEST(pkg, testThetaDeg);
  DEF_TEST(pkg, testSetThetaDeg);

  GVX_PKG_RETURN(pkg);
}

// Need these to avoid dyld errors on Mac OS X
extern "C" int Vectwotest_SafeInit(Tcl_Interp*) { return 1; }

extern "C" int Vectwotest_Unload(Tcl_Interp* interp, int /*flags*/)
{
GVX_TRACE("Vectwotest_Unload");
  return Tcl::Pkg::unloadDestroy(interp, "Vectwotest");
}

extern "C" int Vectwotest_SafeUnload(Tcl_Interp*, int /*flags*/) { return 1; }

static const char vcid_groovx_pkgs_whitebox_vectwotest_cc_utc20050626084022[] = "$Id$ $HeadURL$";
#endif // !GROOVX_PKGS_WHITEBOX_VECTWOTEST_CC_UTC20050626084022_DEFINED
