///////////////////////////////////////////////////////////////////////
//
// trace.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Jan-99
// written: Tue Aug  7 17:39:25 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TRACE_CC_DEFINED
#define TRACE_CC_DEFINED

#define LOCAL_PROF
#include "util/trace.h"

#include "util/minivec.h"

#include <cstdio>
#include <fstream.h>
#include <iostream.h>
#include <iomanip.h>
#include <sys/resource.h>

#define TRACE_WALL_CLOCK_TIME
//  #define TRACE_CPU_TIME

namespace
{
  inline void getTime(timeval& result)
  {
#if defined(TRACE_WALL_CLOCK_TIME)
    gettimeofday(&result, NULL);
#elif defined(TRACE_CPU_TIME)
    rusage ru;
    getrusage(RUSAGE_SELF, &ru);
    result = ru.ru_utime;
#endif
  }

  inline bool shouldTrace(bool val) { return val; }
//    inline bool shouldTrace(bool val) { return true; }
}

int MAX_TRACE_LEVEL = 12;

int TRACE_LEVEL = 0;

Util::Trace::Mode TRACE_MODE = Util::Trace::RUN;

namespace
{
  const char* PDATA_FILE = "prof.out";

  STD_IO::ofstream* PDATA_STREAM = new STD_IO::ofstream(PDATA_FILE);

  void waitOnStep()
  {
    static char dummyBuffer[256];

    STD_IO::cerr << "?" << STD_IO::flush;
    STD_IO::cin >> dummyBuffer;

    switch (dummyBuffer[0])
      {
      case 'r':
      case 'R':
        Util::Trace::setMode(Util::Trace::RUN);
        break;
      default:
        break;
      }
  }

  minivec<Util::Prof*> callStack;

  minivec<Util::Prof*> allProfs;
}

Util::Prof::Prof(const char* s) :
  funcName(s), callCount(0), totalTime()
{
  totalTime.tv_sec = 0;
  totalTime.tv_usec = 0;

  allProfs.push_back(this);
}

Util::Prof::~Prof()
{
  if (PDATA_STREAM->good())
    {
      printProfData(*PDATA_STREAM);
    }
  else
    {
      STD_IO::cerr << "profile stream not good\n";
    }
}

void Util::Prof::printProfData(ostream& os) const
{
  os << setw(14) << long(avgTime()) << '\t'
     << setw(5) << count() << '\t'
     << setw(14) << long(avgTime()) * count() << '\t'
     << funcName << endl;
}

void Util::Prof::resetAllProfData()
{
  for (unsigned int i = 0; i < allProfs.size(); ++i)
    {
      allProfs[i]->callCount = 0;
      allProfs[i]->totalTime.tv_sec = 0;
      allProfs[i]->totalTime.tv_usec = 0;
    }
}

void Util::Prof::printAllProfData(ostream& os)
{
  for (unsigned int i = 0; i < allProfs.size(); ++i)
    {
      allProfs[i]->printProfData(os);
    }
}

void Util::Trace::printStackTrace(ostream& os)
{
  os << "stack trace:\n";

  unsigned int i = 0;
  unsigned int ri = callStack.size()-1;

  for (; i < callStack.size(); ++i, --ri)
    {
      os << "\t[" << i << "] " << callStack[ri]->name() << '\n';
    }

  os << flush;
}

void Util::Trace::printStackTrace()
{
  printf("stack trace:\n");

  unsigned int i = 0;
  unsigned int ri = callStack.size()-1;

  for (; i < callStack.size(); ++i, --ri)
    {
      printf("\t[%d] %s\n", int(i), callStack[ri]->name());
    }
}

void Util::Trace::setMode(Mode new_mode)
{
  TRACE_MODE = new_mode;
}

Util::Trace::Mode Util::Trace::getMode()
{
  return TRACE_MODE;
}

Util::Trace::Trace(Prof& p, bool useMsg) :
  prof(p),
  start(), finish(), elapsed(),
  giveTraceMsg(shouldTrace(useMsg))
{
  if (giveTraceMsg)
    {
      printIn();
    }

  callStack.push_back(&p);

  getTime(start);
}

Util::Trace::~Trace()
{
  getTime(finish);
  elapsed.tv_sec = finish.tv_sec - start.tv_sec;
  elapsed.tv_usec = finish.tv_usec - start.tv_usec;
  prof.add(elapsed);
  if (giveTraceMsg)
    {
      printOut();
    }
  callStack.pop_back();
}

void Util::Trace::printIn()
{

  if (TRACE_LEVEL < MAX_TRACE_LEVEL)
    {
      for (int i=0; i < TRACE_LEVEL; ++i)
        STD_IO::cerr << TRACE_TAB;
      STD_IO::cerr << "entering " << prof.name() << "...";

      if (Util::Trace::getMode() == STEP)
        {
          waitOnStep();
        }
      else
        {
          STD_IO::cerr << '\n';
        }
    }
  ++TRACE_LEVEL;
}

void Util::Trace::printOut()
{
  --TRACE_LEVEL;
  if (TRACE_LEVEL < MAX_TRACE_LEVEL)
    {
      for (int i=0; i < TRACE_LEVEL; ++i)
        STD_IO::cerr << TRACE_TAB;
      STD_IO::cerr << "leaving " << prof.name() << ".";

      if (Util::Trace::getMode() == STEP)
        {
          waitOnStep();
        }
      else
        {
          STD_IO::cerr << '\n';
        }

  }
  if (TRACE_LEVEL == 0) STD_IO::cerr << '\n';
}

static const char vcid_trace_cc[] = "$Header$";
#endif // TRACE_CC_DEFINED
