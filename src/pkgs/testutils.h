///////////////////////////////////////////////////////////////////////
//
// testutils.h
//
// Copyright (c) 2001-2003 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Sep 10 18:57:59 2001
// written: Mon Jan 13 11:04:47 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TESTUTILS_H_DEFINED
#define TESTUTILS_H_DEFINED

#include "util/error.h"

#include "tcl/tclpkg.h"

namespace
{
  void testRequireImpl(bool expr, const char* exprString,
                       const char* file, int line)
  {
    if (!expr)
      throw Util::Error(fstring(file, "@", line,
                                ": failed test of: ",exprString));
  }
}

#define TEST_REQUIRE(expr) \
  testRequireImpl(bool(expr), #expr, __FILE__, __LINE__);

#define DEF_TEST(pkg, func) pkg->def(#func, "", &func)

static const char vcid_testutils_h[] = "$Header$";
#endif // !TESTUTILS_H_DEFINED
