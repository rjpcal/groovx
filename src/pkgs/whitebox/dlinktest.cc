///////////////////////////////////////////////////////////////////////
//
// dlinktest.cc
//
// Copyright (c) 2001-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Mon Sep 10 18:16:43 2001
// commit: $Id$
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

#ifndef DLINKTEST_CC_DEFINED
#define DLINKTEST_CC_DEFINED

#include "util/dlink_list.h"

#include "pkgs/testutils.h"

#include "util/trace.h"

namespace
{
  void testEmptyLength()
  {
    dlink_list<int> dl;
    TEST_REQUIRE(dl.size() == 0);
  }

  void testPushFront()
  {
    dlink_list<int> dl;
    dl.push_front(3);
    TEST_REQUIRE(dl.front() == 3);
  }

  void testPushBack()
  {
    dlink_list<int> dl;
    dl.push_front(3);
    dl.push_back(-1);
    dl.push_back(6);
    TEST_REQUIRE(dl.back() == 6);
  }

  void testInsertBegin()
  {
    dlink_list<int> dl;
    dl.push_front(1);
    dl.push_front(2);
    dl.push_front(3);

    dlink_list<int>::iterator i = dl.begin();
    dl.insert(i, 0);
    TEST_REQUIRE(dl.front() == 0);
  }

  void testInsertMiddle()
  {
    dlink_list<int> dl;
    dl.push_front(1);
    dl.push_front(2);
    dl.push_front(3);

    dlink_list<int>::iterator i = dl.begin();
    dl.insert(++i, 0);
    dl.pop_front();
    TEST_REQUIRE(dl.front() == 0);
  }

  void testInsertEnd()
  {
    dlink_list<int> dl;
    dl.push_front(1);
    dl.push_front(2);
    dl.push_front(3);

    dlink_list<int>::iterator i = dl.end();
    dl.insert(i, 0);
    TEST_REQUIRE(dl.back() == 0);
  }

  void testFindSuccess()
  {
    dlink_list<int> dl;
    dl.push_front(1);
    dl.push_front(2);
    dl.push_front(3);

    dlink_list<int>::iterator i = dl.find(2);
    TEST_REQUIRE(*i == 2);
  }

  void testFindFailure()
  {
    dlink_list<int> dl;
    dl.push_front(1);
    dl.push_front(2);
    dl.push_front(3);

    dlink_list<int>::iterator i = dl.find(-1);
    TEST_REQUIRE(i == dl.end());
  }

  void testClear()
  {
    dlink_list<int> dl;

    dl.push_back(1);
    dl.push_back(2);
    dl.push_back(3);

    dl.clear();

    TEST_REQUIRE(dl.begin() == dl.end());
  }
}

extern "C"
int Dlinktest_Init(Tcl_Interp* interp)
{
DOTRACE("Dlinktest_Init");

  PKG_CREATE(interp, "Dlinktest", "$Revision$");

  DEF_TEST(pkg, testEmptyLength);
  DEF_TEST(pkg, testPushFront);
  DEF_TEST(pkg, testPushBack);
  DEF_TEST(pkg, testInsertBegin);
  DEF_TEST(pkg, testInsertMiddle);
  DEF_TEST(pkg, testInsertEnd);
  DEF_TEST(pkg, testFindSuccess);
  DEF_TEST(pkg, testFindFailure);
  DEF_TEST(pkg, testClear);

  PKG_RETURN;
}

// Need this to avoid dyld errors on Mac OS X
extern "C" int Dlinktest_SafeInit(Tcl_Interp*) { return 1; }

static const char vcid_dlinktest_cc[] = "$Header$";
#endif // !DLINKTEST_CC_DEFINED
