///////////////////////////////////////////////////////////////////////
//
// dlinktest.cc
//
// Copyright (c) 2001-2003 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Sep 10 18:16:43 2001
// written: Mon Jan 13 11:04:47 2003
// $Id$
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

  Tcl::Pkg* pkg = new Tcl::Pkg(interp, "Dlinktest", "$Revision$");

  DEF_TEST(pkg, testEmptyLength);
  DEF_TEST(pkg, testPushFront);
  DEF_TEST(pkg, testPushBack);
  DEF_TEST(pkg, testInsertBegin);
  DEF_TEST(pkg, testInsertMiddle);
  DEF_TEST(pkg, testInsertEnd);
  DEF_TEST(pkg, testFindSuccess);
  DEF_TEST(pkg, testFindFailure);
  DEF_TEST(pkg, testClear);

  return pkg->initStatus();
}

static const char vcid_dlinktest_cc[] = "$Header$";
#endif // !DLINKTEST_CC_DEFINED
