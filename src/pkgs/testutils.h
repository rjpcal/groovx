///////////////////////////////////////////////////////////////////////
//
// testutils.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Sep 10 18:57:59 2001
// written: Fri Jan 18 16:06:56 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TESTUTILS_H_DEFINED
#define TESTUTILS_H_DEFINED

#include "util/error.h"

#include "tcl/tclpkg.h"

#define TEST_REQUIRE(expr) \
    if (!(expr)) \
      throw Util::Error(fstring("test for '", #expr, "' failed"));

#define DEF_TEST(pkg, func) pkg->def(#func, "", &func)

static const char vcid_testutils_h[] = "$Header$";
#endif // !TESTUTILS_H_DEFINED
