///////////////////////////////////////////////////////////////////////
// trace.cc
// Rob Peters 
// created: Jan-99
// written: Wed Jun 30 17:58:58 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef TRACE_CC_DEFINED
#define TRACE_CC_DEFINED

#define LOCAL_PROF
#include "trace.h"

#include <iomanip.h>

int TRACE_LEVEL = 0;

Prof::~Prof() { 
  cout << setw(10) << long(avgTime()) << " " << setw(5) << count()
		 << " " << setw(12) << long(avgTime()) * count()
		 << " " << funcName << endl;
}

static const char vcid_trace_cc[] = "$Header$";
#endif // TRACE_CC_DEFINED
