///////////////////////////////////////////////////////////////////////
//
// debug.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Oct  9 18:48:38 2000
// written: Wed Jul 31 17:19:52 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef DEBUG_CC_DEFINED
#define DEBUG_CC_DEFINED

#include "util/debug.h"

#include <cstdlib>
#include <iostream>

#define LOCAL_PROF
#include "util/trace.h"

void Debug::AssertImpl(const char* what, const char* where, int line_no)
{
  STD_IO::cerr << "Assertion failed: '" << what << "' in "
               << where << " line " << line_no << '\n';
  Util::BackTrace::current().print(STD_IO::cerr);
  abort();
}

void Debug::PreconditionImpl(const char* what, const char* where, int line_no)
{
  STD_IO::cerr << "Precondition failed: '" << what << "' in "
               << where << " line " << line_no << '\n';
  Util::BackTrace::current().print(STD_IO::cerr);
  abort();
}

void Debug::PostconditionImpl(const char* what, const char* where, int line_no)
{
  STD_IO::cerr << "Postcondition failed: '" << what << "' in "
               << where << " line " << line_no << '\n';
  Util::BackTrace::current().print(STD_IO::cerr);
  abort();
}

void Debug::InvariantImpl(const char* what, const char* where, int line_no)
{
  STD_IO::cerr << "Invariant failed: '" << what << "' in "
               << where << " line " << line_no << '\n';
  Util::BackTrace::current().print(STD_IO::cerr);
  abort();
}

static const char vcid_debug_cc[] = "$Header$";
#endif // !DEBUG_CC_DEFINED
