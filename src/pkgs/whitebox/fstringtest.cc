///////////////////////////////////////////////////////////////////////
//
// fstringtest.cc
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jun 24 17:14:36 2002
// written: Mon Jun 24 17:40:10 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FSTRINGTEST_CC_DEFINED
#define FSTRINGTEST_CC_DEFINED

#include "util/strings.h"

#include "pkgs/testutils.h"

#include <cstring>

#include "util/trace.h"

namespace
{
  void testEmptyLength()
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
}

extern "C"
int Fstringtest_Init(Tcl_Interp* interp)
{
DOTRACE("Fstringtest_Init");

  Tcl::Pkg* pkg = new Tcl::Pkg(interp, "Fstringtest", "$Revision$");

  DEF_TEST(pkg, testEmptyLength);
  DEF_TEST(pkg, testConstruct1);

  return pkg->initStatus();
}

static const char vcid_fstringtest_cc[] = "$Header$";
#endif // !FSTRINGTEST_CC_DEFINED
