///////////////////////////////////////////////////////////////////////
//
// debug.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Oct  9 18:48:38 2000
// written: Mon Oct  9 19:27:40 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef DEBUG_CC_DEFINED
#define DEBUG_CC_DEFINED

#include "util/debug.h"

#include <cstdlib>
#include <iostream.h>

#include "util/trace.h"

void Debug::AssertImpl(const char* what, const char* where, int line_no) {
  cerr << "Assertion failed: '" << what << "' in "
		 << where << " line " << line_no << '\n';
  Util::Trace::printStackTrace(cerr);
  abort();
}

void Debug::PreconditionImpl(const char* what, const char* where, int line_no) {
  cerr << "Precondition failed: '" << what << "' in "
		 << where << " line " << line_no << '\n';
  Util::Trace::printStackTrace(cerr);
  abort();
}

void Debug::PostconditionImpl(const char* what, const char* where, int line_no) {
  cerr << "Postcondition failed: '" << what << "' in "
		 << where << " line " << line_no << '\n';
  Util::Trace::printStackTrace(cerr);
  abort();
}

void Debug::InvariantImpl(const char* what, const char* where, int line_no) {
  cerr << "Invariant failed: '" << what << "' in "
		 << where << " line " << line_no << '\n';
  Util::Trace::printStackTrace(cerr);
  abort();
}

static const char vcid_debug_cc[] = "$Header$";
#endif // !DEBUG_CC_DEFINED
