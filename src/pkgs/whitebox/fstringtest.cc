///////////////////////////////////////////////////////////////////////
//
// fstringtest.cc
//
// Copyright (c) 2002-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Mon Jun 24 17:14:36 2002
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

#ifndef FSTRINGTEST_CC_DEFINED
#define FSTRINGTEST_CC_DEFINED

#include "pkgs/whitebox/fstringtest.h"

#include "tcl/tclpkg.h"

#include "util/fstring.h"
#include "util/unittest.h"

#include <cstring>
#include <sstream>

#include "util/trace.h"

using rutz::char_range;
using rutz::fstring;

namespace
{
  void testDefaultConstruct()
  {
    fstring s;
    TEST_REQUIRE(s.is_empty());
    TEST_REQUIRE(s.length() == 0);
    TEST_REQUIRE(s.c_str() != 0);
    TEST_REQUIRE(s.c_str()[0] == '\0');
  }

  void testConstruct1()
  {
    fstring s("hello world!");
    TEST_REQUIRE(!s.is_empty());
    TEST_REQUIRE(s.length() == 12);
    TEST_REQUIRE(s.c_str() != 0);
    TEST_REQUIRE(s.c_str()[s.length()] == '\0');
    TEST_REQUIRE(std::strlen(s.c_str()) == s.length());
    TEST_REQUIRE(std::strcmp(s.c_str(), "hello world!") == 0);
    TEST_REQUIRE(s == "hello world!");
    TEST_REQUIRE(s != "hello world");
    TEST_REQUIRE(s != "hello world!!");
    TEST_REQUIRE(s != "hello world?");
    TEST_REQUIRE(s != "");
    TEST_REQUIRE(s.ends_with("ld!"));
  }

  void testConstruct2()
  {
    fstring s("foo", " fighters!");
    TEST_REQUIRE(s.length() == 13);
    TEST_REQUIRE(s == "foo fighters!");
  }

  void testConstructNum()
  {
    fstring s("double val: ", 1.234, ", int val: ", 1234);
    TEST_REQUIRE(s == "double val: 1.234, int val: 1234");
  }

  void testConstructCharRange()
  {
    // note that this array is NOT null-terminated
    const char init[19] =
      {
        't', 'h', 'e', ' ', 'q', 'u', 'i', 'c', 'k', ' ',
        'b', 'r', 'o', 'w', 'n', ' ', 'f', 'o', 'x'
      };

    {
      fstring s((char_range(&init[0], 0)));
      TEST_REQUIRE(s == "");
    }

    {
      fstring s((char_range(&init[0], 1)));
      TEST_REQUIRE(s == "t");
    }

    {
      fstring s((char_range(&init[0], 4)));
      TEST_REQUIRE(s == "the ");
    }

    {
      fstring s((char_range(&init[4], 5)));
      TEST_REQUIRE(s == "quick");
    }

    {
      fstring s((char_range(&init[0], 19)));
      TEST_REQUIRE(s == "the quick brown fox");
    }
  }

  void testSwap()
  {
    fstring s1("foo");
    fstring s2("bar");

    s1.swap(s2);

    TEST_REQUIRE(s1 == "bar");
    TEST_REQUIRE(s2 == "foo");
  }

  void testAssign1()
  {
    fstring s1("foo");

    s1 = "bar";

    TEST_REQUIRE(s1 == "bar");
  }

  void testAssign2()
  {
    fstring s1("foo");

    s1 = fstring("bar");

    TEST_REQUIRE(s1 == "bar");
  }

  void testAssignEmptyString()
  {
    fstring s1("foo");

    s1 = "";

    TEST_REQUIRE(s1 == "");
  }

  void testAssignNullString()
  {
    fstring s1("foo");

    s1 = static_cast<const char*>(0);

    TEST_REQUIRE(s1 == "");
  }

  void testCopyOnWrite()
  {
    fstring s1("copyme");
    fstring s2 = s1;

    TEST_REQUIRE(s1 == s2);

    s1.append(" orig");

    TEST_REQUIRE(s1 != s2);
    TEST_REQUIRE(s1 == "copyme orig");
    TEST_REQUIRE(s2 == "copyme");
  }

  void testLessThan()
  {
    fstring s("apple");

    TEST_REQUIRE(s < "banana");
    TEST_REQUIRE(s < "apples");
    TEST_REQUIRE(s < "azure");
  }

  void testGreaterThan()
  {
    fstring s("pineapple");

    TEST_REQUIRE(s > "orange");
    TEST_REQUIRE(s > "pine");
    TEST_REQUIRE(s > "panini");
  }

  void testRead()
  {
    const std::string orig("orange panini");
    std::istringstream in(orig);

    fstring dummy; (void)dummy; // this is here to make sure that the
                                // "empty_string_rep" is shared,
                                // forcing us to make sure that the
                                // real "out" fstring uniquifies
                                // itself before trying to read from
                                // the istringstream
    fstring out;
    in >> out;
    TEST_REQUIRE_EQ(out, "orange");
  }

  void testReadsome()
  {
    const std::string orig("azure banana");
    std::istringstream in(orig);

    fstring dummy; (void)dummy; // see comment in testRead();

    fstring out;
    out.readsome(in, 10);
    TEST_REQUIRE_EQ(out, "azure bana");
  }

  void testReadline1()
  {
    const std::string orig("marooned on\nan island!\nforever\n");
    std::istringstream in(orig);

    fstring dummy; (void)dummy; // see comment in testRead();

    fstring out;
    out.readline(in);
    TEST_REQUIRE_EQ(out, "marooned on");
  }

  void testReadline2()
  {
    const std::string orig("marooned on\nan island!\nforever\n");
    std::istringstream in(orig);

    fstring dummy; (void)dummy; // see comment in testRead();

    fstring out;
    out.readline(in, '!');
    TEST_REQUIRE_EQ(out, "marooned on\nan island");
  }

  void testWrite()
  {
    fstring orig("blue\ncrayon\n\tskies");

    std::ostringstream out;

    out << orig;

    TEST_REQUIRE_EQ(out.str().c_str(), fstring("blue\ncrayon\n\tskies"));
  }
}

extern "C"
int Fstringtest_Init(Tcl_Interp* interp)
{
DOTRACE("Fstringtest_Init");

  PKG_CREATE(interp, "Fstringtest", "4.$Revision$");

  DEF_TEST(pkg, testDefaultConstruct);
  DEF_TEST(pkg, testConstruct1);
  DEF_TEST(pkg, testConstruct2);
  DEF_TEST(pkg, testConstructNum);
  DEF_TEST(pkg, testConstructCharRange);
  DEF_TEST(pkg, testSwap);
  DEF_TEST(pkg, testAssign1);
  DEF_TEST(pkg, testAssign2);
  DEF_TEST(pkg, testAssignEmptyString);
  DEF_TEST(pkg, testAssignNullString);
  DEF_TEST(pkg, testCopyOnWrite);
  DEF_TEST(pkg, testLessThan);
  DEF_TEST(pkg, testGreaterThan);
  DEF_TEST(pkg, testRead);
  DEF_TEST(pkg, testReadsome);
  DEF_TEST(pkg, testReadline1);
  DEF_TEST(pkg, testReadline2);
  DEF_TEST(pkg, testWrite);

  PKG_RETURN;
}

// Need these to avoid dyld errors on Mac OS X
extern "C" int Fstringtest_SafeInit(Tcl_Interp*) { return 1; }

extern "C" int Fstringtest_Unload(Tcl_Interp* interp, int /*flags*/)
{
DOTRACE("Fstringtest_Unload");
  return Tcl::Pkg::unloadDestroy(interp, "Fstringtest");
}

extern "C" int Fstringtest_SafeUnload(Tcl_Interp*, int /*flags*/) { return 1; }

static const char vcid_fstringtest_cc[] = "$Id$ $URL$";
#endif // !FSTRINGTEST_CC_DEFINED
