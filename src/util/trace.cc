///////////////////////////////////////////////////////////////////////
//
// trace.cc
//
// Copyright (c) 1999-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Mon Jan  4 08:00:00 1999
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef TRACE_CC_DEFINED
#define TRACE_CC_DEFINED

#define LOCAL_PROF
#include "util/trace.h"

#include "util/backtrace.h"
#include "util/staticstack.h"

#include <algorithm>
#include <cstdio>
#include <fstream>
#include <functional>
#include <iostream>
#include <iomanip>
#include <new> // for std::nothrow

#include "util/debug.h"
DBG_REGISTER

///////////////////////////////////////////////////////////////////////
//
// File-scope helper functions and data
//
///////////////////////////////////////////////////////////////////////

#define TRACE_WALL_CLOCK_TIME
//#define TRACE_CPU_TIME

namespace
{

  inline Util::Time getTime() throw()
  {
#if defined(TRACE_WALL_CLOCK_TIME)
    return Util::Time::wallClockNow();
#elif defined(TRACE_CPU_TIME)
    return Util::Time::rusageUserNow() + Util::Time::rusageSysNow();
#else
#  error must define either TRACE_WALL_CLOCK_TIME or TRACE_CPU_TIME
#endif
  }

  unsigned int MAX_TRACE_LEVEL = 20;

  bool GLOBAL_TRACE = false;

  const char* PDATA_FILE = "prof.out";

  bool PRINT_AT_EXIT = true;

  void waitOnStep() throw()
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

  typedef static_stack<Util::Prof*, 2048> ProfVec;

  ProfVec& allProfs() throw()
  {
    static ProfVec* ptr = 0;
    if (ptr == 0)
      {
        ptr = new (std::nothrow) ProfVec;

        if (ptr == 0)
          Panic("memory allocation failed");
      }
    return *ptr;
  }
}

///////////////////////////////////////////////////////////////////////
//
// Util::Prof member definitions
//
///////////////////////////////////////////////////////////////////////

Util::Prof::Prof(const char* s, const char* fname, int lineno)  throw():
  itsFuncName(s),
  itsSourceFileName(fname),
  itsSourceLineNo(lineno)
{
  reset();

  allProfs().push(this);
}

Util::Prof::~Prof() throw()
{
  if (PRINT_AT_EXIT)
    {
      static FILE* file = 0;
      static bool inited = false;

      if (!inited)
        {
          file = fopen(PDATA_FILE, "w");

          // need this extra state flag since it's possible that the
          // fopen() call above fails, so we can't simply check (file!=0)
          // to see if initialization has already been tried
          inited = true;

          if (file == 0)
            {
              fprintf(stderr, "couldn't open profile file for writing\n");
            }
        }

      if (file != 0)
        {
          printProfData(file);
        }
    }
}

void Util::Prof::reset() throw()
{
  itsCallCount = 0;
  itsTotalTime.reset();
  itsChildrenTime.reset();
}

unsigned int Util::Prof::count() const throw()
{
  return itsCallCount;
}

void Util::Prof::add(const Util::Time& t) throw()
{
  itsTotalTime += t;
  ++itsCallCount;
}

void Util::Prof::addChildTime(const Util::Time& t) throw()
{
  itsChildrenTime += t;
}

const char* Util::Prof::name() const throw()
{
  return itsFuncName;
}

const char* Util::Prof::srcFileName() const throw()
{
  return itsSourceFileName;
}

int Util::Prof::srcLineNo() const throw()
{
  return itsSourceLineNo;
}

double Util::Prof::totalTime() const throw()
{
  return (itsCallCount > 0) ?
    itsTotalTime.usec()
    : 0.0;
}

double Util::Prof::selfTime() const throw()
{
  return (itsCallCount > 0)
    ? itsTotalTime.usec() - itsChildrenTime.usec()
    : 0.0;
}

double Util::Prof::avgTime() const throw()
{
  return itsCallCount > 0 ? (totalTime() / itsCallCount) : 0.0;
}

void Util::Prof::printProfData(FILE* file) const throw()
{
  Assert(file != 0);

  fprintf(file, "%10ld %6u %10ld %10ld %s\n",
          long(avgTime()), count(),
          long(selfTime()), long(totalTime()),
          itsFuncName);
}

void Util::Prof::printProfData(std::ostream& os) const throw()
{
  os.exceptions(STD_IO::ios::goodbit);

  os << std::setw(10) << long(avgTime()) << ' '
     << std::setw(6) << count() << ' '
     << std::setw(10) << long(selfTime()) << ' '
     << std::setw(10) << long(totalTime()) << ' '
     << itsFuncName << '\n';
}

void Util::Prof::printAtExit(bool yes_or_no) throw()
{
  PRINT_AT_EXIT = yes_or_no;
}

void Util::Prof::resetAllProfData() throw()
{
  std::for_each(allProfs().begin(), allProfs().end(),
                std::mem_fun(&Util::Prof::reset));
}

namespace
{
  bool compareTotalTime(Util::Prof* p1, Util::Prof* p2) throw()
  {
    return p1->totalTime() < p2->totalTime();
  }
}

void Util::Prof::printAllProfData(FILE* file) throw()
{
  std::stable_sort(allProfs().begin(), allProfs().end(), compareTotalTime);

  for (unsigned int i = 0; i < allProfs().size(); ++i)
    {
      if (allProfs()[i]->count() > 0)
        allProfs()[i]->printProfData(file);
    }
}

void Util::Prof::printAllProfData(STD_IO::ostream& os) throw()
{
  std::stable_sort(allProfs().begin(), allProfs().end(), compareTotalTime);

  for (unsigned int i = 0; i < allProfs().size(); ++i)
    {
      if (allProfs()[i]->count() > 0)
        allProfs()[i]->printProfData(os);
    }
}

///////////////////////////////////////////////////////////////////////
//
// Util::Trace member definitions
//
///////////////////////////////////////////////////////////////////////

Util::Trace::Mode TRACE_MODE = Util::Trace::RUN;

void Util::Trace::setMode(Mode new_mode) throw()
{
  TRACE_MODE = new_mode;
}

Util::Trace::Mode Util::Trace::getMode() throw()
{
  return TRACE_MODE;
}

bool Util::Trace::getGlobalTrace() throw()
{
  return GLOBAL_TRACE;
}

void Util::Trace::setGlobalTrace(bool on_off) throw()
{
  GLOBAL_TRACE = on_off;
}

unsigned int Util::Trace::getMaxLevel() throw()
{
  return MAX_TRACE_LEVEL;
}

void Util::Trace::setMaxLevel(unsigned int lev) throw()
{
  MAX_TRACE_LEVEL = lev;
}

Util::Trace::Trace(Prof& p, bool useMsg) throw() :
  itsProf(p),
  itsStart(),
  itsGiveTraceMsg(useMsg),
  itsShouldPop(true)
{
  if (GLOBAL_TRACE || itsGiveTraceMsg)
    printIn();

  itsShouldPop = Util::BackTrace::current().push(&p);

  itsStart = getTime();
}

Util::Trace::~Trace() throw()
{
  Util::Time finish = getTime();
  Util::Time elapsed = finish - itsStart;
  itsProf.add(elapsed);

  if (itsShouldPop)
    {
      Util::BackTrace& c = Util::BackTrace::current();
      c.pop();
      Util::Prof* parent = c.top();
      if (parent != 0)
        parent->addChildTime(elapsed);
    }

  if (GLOBAL_TRACE || itsGiveTraceMsg)
    printOut();
}

void Util::Trace::printIn() throw()
{
  const unsigned int n = Util::BackTrace::current().size();

  if (n < MAX_TRACE_LEVEL)
    {
      for (unsigned int i = 0; i < n; ++i)
        {
          STD_IO::cerr << "|   ";
        }
      // Test whether n has 1, 2, or >3 digits
      if (n < 10)
        STD_IO::cerr << n << "-->> ";
      else if (n < 100)
        STD_IO::cerr << n << "->> ";
      else
        STD_IO::cerr << n << ">> ";

      STD_IO::cerr << itsProf.name();

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

void Util::Trace::printOut() throw()
{
  const unsigned int n = Util::BackTrace::current().size();

  if (n < MAX_TRACE_LEVEL)
    {
      for (unsigned int i = 0; i < n; ++i)
        {
          STD_IO::cerr << "|   ";
        }
      STD_IO::cerr << "| <<--";

      STD_IO::cerr << itsProf.name();

      if (Util::Trace::getMode() == STEP)
        {
          waitOnStep();
        }
      else
        {
          STD_IO::cerr << '\n';
        }
    }

  if (Util::BackTrace::current().size() == 0)
    STD_IO::cerr << '\n';
}

static const char vcid_trace_cc[] = "$Header$";
#endif // TRACE_CC_DEFINED
