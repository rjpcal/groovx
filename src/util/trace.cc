///////////////////////////////////////////////////////////////////////
//
// trace.cc
// Rob Peters 
// created: Jan-99
// written: Sat Sep 23 15:32:24 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TRACE_CC_DEFINED
#define TRACE_CC_DEFINED

#define LOCAL_PROF
#include "util/trace.h"

#include <fstream.h>
#include <iostream.h>
#include <iomanip.h>

int MAX_TRACE_LEVEL = 6;

int TRACE_LEVEL = 0;

Util::Trace::Mode TRACE_MODE = Util::Trace::RUN;

namespace {
  const char* PDATA_FILE = "prof.out";

  STD_IO::ofstream* PDATA_STREAM = new STD_IO::ofstream(PDATA_FILE);

  void waitOnStep() {
	 static char dummyBuffer[256];

	 cerr << "?" << flush;
	 cin >> dummyBuffer;

	 switch (dummyBuffer[0]) {
	 case 'r':
	 case 'R':
		Util::Trace::setMode(Util::Trace::RUN);
		break;
	 default:
		break;
	 }
  }

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

void Util::Trace::setMode(Mode new_mode) {
  TRACE_MODE = new_mode;
}

Util::Trace::Mode Util::Trace::getMode() {
  return TRACE_MODE;
}

void Util::Trace::printIn() {

  if (TRACE_LEVEL < MAX_TRACE_LEVEL) {
	 for (int i=0; i < TRACE_LEVEL; ++i)
		cerr << TRACE_TAB;
	 cerr << "entering " << prof.name() << "...";

	 if (Util::Trace::getMode() == STEP) {
		waitOnStep();
	 }
	 else {
		cerr << endl;
	 }
  }
  ++TRACE_LEVEL;
}

void Util::Trace::printOut() {
  --TRACE_LEVEL;
  if (TRACE_LEVEL < MAX_TRACE_LEVEL) {
	 for (int i=0; i < TRACE_LEVEL; ++i)
		cerr << TRACE_TAB;
	 cerr << "leaving " << prof.name() << ".";

	 if (Util::Trace::getMode() == STEP) {
		waitOnStep();
	 }
	 else {
		cerr << endl;
	 }

  }
  if (TRACE_LEVEL == 0) cerr << endl;
}

static const char vcid_trace_cc[] = "$Header$";
#endif // TRACE_CC_DEFINED
