///////////////////////////////////////////////////////////////////////
// trace.cc
// Rob Peters 
// created: Jan-99
// written: Tue Feb  8 19:16:16 2000
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef TRACE_CC_DEFINED
#define TRACE_CC_DEFINED

#define LOCAL_PROF
#include "trace.h"

#include <iomanip.h>

int MAX_TRACE_LEVEL = 6;

int TRACE_LEVEL = 0;

Util::Prof::~Prof() {
  cout << setw(10) << long(avgTime()) << " " << setw(5) << count()
		 << " " << setw(12) << long(avgTime()) * count()
		 << " " << funcName << endl;
}

static const char vcid_trace_cc[] = "$Header$";
#endif // TRACE_CC_DEFINED
