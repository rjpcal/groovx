///////////////////////////////////////////////////////////////////////
//
// trace.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jan  4 08:00:00 1999
// written: Thu Feb  7 14:51:32 2002
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

  unsigned int MAX_TRACE_LEVEL = 12;

  bool GLOBAL_TRACE = false;

  const char* PDATA_FILE = "prof.out";

  STD_IO::ofstream* PDATA_STREAM = new STD_IO::ofstream(PDATA_FILE);

  bool PRINT_AT_EXIT = true;

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

  typedef minivec<Util::Prof*> ProfVec;

  ProfVec& callStack()
  {
    static ProfVec* ptr = 0;
    if (ptr == 0)
      {
        ptr = new ProfVec;
      }
    return *ptr;
  }

  ProfVec& allProfs()
  {
    static ProfVec* ptr = 0;
    if (ptr == 0)
      {
        ptr = new ProfVec;
      }
    return *ptr;
  }
}


///////////////////////////////////////////////////////////////////////
//
// Util::Prof member definitions
//
///////////////////////////////////////////////////////////////////////

Util::Prof::Prof(const char* s) :
  funcName(s), callCount(0), totalTime()
{
  totalTime.tv_sec = 0;
  totalTime.tv_usec = 0;

  allProfs().push_back(this);
}

Util::Prof::~Prof()
{
  if (PRINT_AT_EXIT)
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
}

int Util::Prof::count() const { return callCount; }

void Util::Prof::add(timeval t)
{
  totalTime.tv_sec += t.tv_sec;
  totalTime.tv_usec += t.tv_usec;
  // avoid overflow
  while (totalTime.tv_usec >= 1000000)
    {
      ++(totalTime.tv_sec);
      totalTime.tv_usec -= 1000000;
    }
  // avoid underflow
  while (totalTime.tv_usec <= -1000000)
    {
      --(totalTime.tv_sec);
      totalTime.tv_usec += 1000000;
    }
  ++callCount;
}

const char* Util::Prof::name() const
{
  return funcName;
}

double Util::Prof::avgTime() const
{
  return (double(totalTime.tv_sec)*1000000 + totalTime.tv_usec)
    / callCount;
}

void Util::Prof::printProfData(ostream& os) const
{
  os << setw(14) << long(avgTime()) << '\t'
     << setw(5) << count() << '\t'
     << setw(14) << long(avgTime()) * count() << '\t'
     << funcName << endl;
}

void Util::Prof::printAtExit(bool yes_or_no)
{
  PRINT_AT_EXIT = yes_or_no;
}

void Util::Prof::resetAllProfData()
{
  for (unsigned int i = 0; i < allProfs().size(); ++i)
    {
      allProfs()[i]->callCount = 0;
      allProfs()[i]->totalTime.tv_sec = 0;
      allProfs()[i]->totalTime.tv_usec = 0;
    }
}

void Util::Prof::printAllProfData(ostream& os)
{
  for (unsigned int i = 0; i < allProfs().size(); ++i)
    {
      allProfs()[i]->printProfData(os);
    }
}

///////////////////////////////////////////////////////////////////////
//
// Util::Trace member definitions
//
///////////////////////////////////////////////////////////////////////

Util::Trace::Mode TRACE_MODE = Util::Trace::RUN;

void Util::Trace::setMode(Mode new_mode)
{
  TRACE_MODE = new_mode;
}

Util::Trace::Mode Util::Trace::getMode()
{
  return TRACE_MODE;
}

bool Util::Trace::getGlobalTrace()
{
  return GLOBAL_TRACE;
}

void Util::Trace::setGlobalTrace(bool on_off)
{
  GLOBAL_TRACE = on_off;
}

unsigned int Util::Trace::getMaxLevel()
{
  return MAX_TRACE_LEVEL;
}

void Util::Trace::setMaxLevel(unsigned int lev)
{
  MAX_TRACE_LEVEL = lev;
}

void Util::Trace::printStackTrace(ostream& os)
{
  os << "stack trace:\n";

  unsigned int i = 0;
  unsigned int ri = callStack().size()-1;

  for (; i < callStack().size(); ++i, --ri)
    {
      os << "\t[" << i << "] " << callStack()[ri]->name() << '\n';
    }

  os << flush;
}

void Util::Trace::printStackTrace()
{
  printf("stack trace:\n");

  unsigned int i = 0;
  unsigned int ri = callStack().size()-1;

  for (; i < callStack().size(); ++i, --ri)
    {
      printf("\t[%d] %s\n", int(i), callStack()[ri]->name());
    }
}

Util::Trace::Trace(Prof& p, bool useMsg) :
  prof(p),
  start(), finish(), elapsed(),
  giveTraceMsg(useMsg)
{
  if (GLOBAL_TRACE || giveTraceMsg)
    {
      printIn();
    }

  callStack().push_back(&p);

  getTime(start);
}

Util::Trace::~Trace()
{
  getTime(finish);
  elapsed.tv_sec = finish.tv_sec - start.tv_sec;
  elapsed.tv_usec = finish.tv_usec - start.tv_usec;
  prof.add(elapsed);
  callStack().pop_back();
  if (GLOBAL_TRACE || giveTraceMsg)
    {
      printOut();
    }
}

void Util::Trace::printIn()
{
  if (callStack().size() < MAX_TRACE_LEVEL)
    {
      if (callStack().size() > 0)
        {
          for (unsigned int i=0; i < callStack().size()-1; ++i)
            {
              STD_IO::cerr << "|   ";
            }
          STD_IO::cerr << "|- ";
        }

      STD_IO::cerr << prof.name();

      if (Util::Trace::getMode() == STEP)
        {
          waitOnStep();
        }
      else
        {
          STD_IO::cerr << '\n';
        }
    }
}

void Util::Trace::printOut()
{
  if (callStack().size() == 0) STD_IO::cerr << '\n';
}

static const char vcid_trace_cc[] = "$Header$";
#endif // TRACE_CC_DEFINED
