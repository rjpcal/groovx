///////////////////////////////////////////////////////////////////////
//
// trace.cc
// Rob Peters 
// created: Jan-99
// written: Tue Oct 10 08:05:47 2000
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
#include <vector>

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

  std::vector<Util::Prof*> callStack;
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

void Util::Trace::printStackTrace(ostream& os) {
  os << "stack trace:\n";
  for (int i = 0; i < callStack.size(); ++i) {
	 os << "\t[" << i << "] " << callStack[i]->name() << '\n';
  }
  os << flush;
}

void Util::Trace::setMode(Mode new_mode) {
  TRACE_MODE = new_mode;
}

Util::Trace::Mode Util::Trace::getMode() {
  return TRACE_MODE;
}

Util::Trace::Trace(Prof& p, bool useMsg) :
  prof(p),
  start(), finish(), elapsed(),
  giveTraceMsg(useMsg)
{
  if (giveTraceMsg)
	 {
		printIn();
	 }

  callStack.push_back(&p);

  gettimeofday(&start, NULL);
}

Util::Trace::~Trace()
{
  gettimeofday(&finish, NULL);
  elapsed.tv_sec = finish.tv_sec - start.tv_sec;
  elapsed.tv_usec = finish.tv_usec - start.tv_usec;
  prof.add(elapsed);
  if (giveTraceMsg)
	 {
		printOut();
	 }
  callStack.pop_back();
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
