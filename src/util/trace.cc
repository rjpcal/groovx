///////////////////////////////////////////////////////////////////////
//
// trace.cc
// Rob Peters 
// created: Jan-99
// written: Mon Mar 13 12:43:52 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TRACE_CC_DEFINED
#define TRACE_CC_DEFINED

#define LOCAL_PROF
#include "trace.h"

#include <fstream.h>
#include <iostream.h>
#include <iomanip.h>

int MAX_TRACE_LEVEL = 6;

int TRACE_LEVEL = 0;

namespace {
  const char* PDATA_FILE = "prof.out";

  ofstream* PDATA_STREAM = new ofstream(PDATA_FILE);
}

Util::Prof::~Prof() {

  if (PDATA_STREAM->good()) {
	 *PDATA_STREAM << setw(10) << long(avgTime()) << " "
						<< setw(5) << count() << " "
						<< setw(12) << long(avgTime()) * count() << " "
						<< funcName << endl;
  }
  else {
	 cerr << "profile stream not good\n";
  }
}

void Util::Trace::printIn() {
  if (TRACE_LEVEL < MAX_TRACE_LEVEL) {
	 for (int i=0; i < TRACE_LEVEL; ++i)
		cerr << TRACE_TAB;
	 cerr << "entering " << prof.name() << "...\n" << flush;
  }
  ++TRACE_LEVEL;
}

void Util::Trace::printOut() {
  --TRACE_LEVEL;
  if (TRACE_LEVEL < MAX_TRACE_LEVEL) {
	 for (int i=0; i < TRACE_LEVEL; ++i)
		cerr << TRACE_TAB;
	 cerr << "leaving " << prof.name() << ".\n" << flush;
  }
  if (TRACE_LEVEL == 0) cerr << endl;
}

static const char vcid_trace_cc[] = "$Header$";
#endif // TRACE_CC_DEFINED
