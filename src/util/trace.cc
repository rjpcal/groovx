///////////////////////////////////////////////////////////////////////
//
// trace.cc
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Mon Jan  4 08:00:00 1999
// written: Wed Mar 19 17:58:51 2003
// $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef TRACE_CC_DEFINED
#define TRACE_CC_DEFINED

#define LOCAL_PROF
#include "util/trace.h"

#include <algorithm>
#include <cstdio>
#include <fstream>
#include <functional>
#include <iostream>
#include <iomanip>
#include <new>

#include "util/debug.h"

#define TRACE_WALL_CLOCK_TIME
//  #define TRACE_CPU_TIME

template <typename T, unsigned int N>
class static_stack
{
public:
  static_stack() throw() : vec(), sz(0) {}

  static_stack(const static_stack& other) throw() :
    vec(), sz(0)
  {
    *this = other;
  }

  static_stack& operator=(const static_stack& other) throw()
  {
    for (unsigned int i = 0; i < other.sz; ++i)
      {
        vec[i] = other.vec[i];
      }

    return *this;
  }

  unsigned int size() const throw() { return sz; }

  static unsigned int capacity() throw() { return N; }

  bool push(T p) throw()
  {
    if (sz >= N)
      return false;

    vec[sz++] = p;
    return true;
  }

  void pop() throw()
  {
    AbortIf(sz == 0);
    --sz;
  }

  T top() const throw()
  {
    return (sz > 0) ? vec[sz-1] : 0;
  }

  T at(unsigned int i) const throw()
  {
    return (i < sz) ? vec[i] : 0;
  }

  T operator[](unsigned int i) const throw() { return at(i); }

  typedef       T*       iterator;
  typedef const T* const_iterator;

  iterator begin() throw() { return &vec[0]; }
  iterator end()   throw() { return &vec[0] + N; }

  const_iterator begin() const throw() { return &vec[0]; }
  const_iterator end()   const throw() { return &vec[0] + N; }

private:
  T vec[N];
  unsigned int sz;
};

///////////////////////////////////////////////////////////////////////
//
// Util::BackTrace member definitions
//
///////////////////////////////////////////////////////////////////////

struct Util::BackTrace::Impl
{
  static_stack<Util::Prof*, 256> vec;
};

Util::BackTrace::BackTrace() throw() :
  rep(new (std::nothrow) Impl)
{
  AbortIf(rep == 0);
}

Util::BackTrace::BackTrace(const BackTrace& other) throw() :
  rep(new (std::nothrow) Impl(*other.rep))
{
  AbortIf(rep == 0);
}

Util::BackTrace& Util::BackTrace::operator=(const BackTrace& other) throw()
{
  *rep = *other.rep;
  return *this;
}

Util::BackTrace::~BackTrace() throw()
{
  delete rep;
}

Util::BackTrace& Util::BackTrace::current() throw()
{
  static Util::BackTrace* ptr = 0;
  if (ptr == 0)
    {
      ptr = new (std::nothrow) Util::BackTrace;

      AbortIf(ptr == 0);
    }
  return *ptr;
}

bool Util::BackTrace::push(Util::Prof* p) throw()
{
  return rep->vec.push(p);
}

void Util::BackTrace::pop() throw()
{
  rep->vec.pop();
}

unsigned int Util::BackTrace::size() const throw()
{
  return rep->vec.size();
}

Util::Prof* Util::BackTrace::top() const throw()
{
  return rep->vec.top();
}

Util::Prof* Util::BackTrace::at(unsigned int i) const throw()
{
  return rep->vec.at(i);
}

void Util::BackTrace::print() const throw()
{
  printf("stack trace:\n");

  const unsigned int end = size();

  unsigned int i = 0;
  unsigned int ri = end-1;

  for (; i < end; ++i, --ri)
    {
      printf("\t[%d] %s\n", int(i), rep->vec.at(ri)->name());
    }
}

void Util::BackTrace::print(STD_IO::ostream& os) const throw()
{
  os.exceptions(STD_IO::ios::goodbit);

  os << "stack trace:\n";

  const unsigned int end = size();

  unsigned int i = 0;
  unsigned int ri = end-1;

  for (; i < end; ++i, --ri)
    {
      os << "\t[" << i << "] " << rep->vec.at(ri)->name() << '\n';
    }

  os << std::flush;
}

///////////////////////////////////////////////////////////////////////
//
// File-scope helper functions and data
//
///////////////////////////////////////////////////////////////////////

namespace
{
  inline Util::Time getTime() throw()
  {
#if defined(TRACE_WALL_CLOCK_TIME)
    return Util::Time::wallClockNow();
#elif defined(TRACE_CPU_TIME)
    return Util::Time::rusageNow();
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
        AbortIf(ptr == 0);
      }
    return *ptr;
  }
}

///////////////////////////////////////////////////////////////////////
//
// Util::Prof member definitions
//
///////////////////////////////////////////////////////////////////////

Util::Prof::Prof(const char* s)  throw():
  itsFuncName(s)
{
  reset();

  allProfs().push(this);
}

Util::Prof::~Prof() throw()
{
  if (PRINT_AT_EXIT)
    {
      static STD_IO::ofstream* stream = 0;
      static bool inited = false;

      if (!inited)
        {
          stream = new (std::nothrow) STD_IO::ofstream(PDATA_FILE);

          // need this extra state flag since it's possible that the new
          // call above fails, so we can't simply check (stream != 0) to
          // see if initialization has already been tried
          inited = true;
        }

      if (stream && stream->good())
        {
          printProfData(*stream);
        }
      else
        {
          STD_IO::cerr << "profile stream not good\n";
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

void Util::Prof::printProfData(std::ostream& os) const throw()
{
  os.exceptions(STD_IO::ios::goodbit);

  os << std::setw(10) << long(avgTime()) << ' '
     << std::setw(6) << count() << ' '
     << std::setw(10) << long(selfTime()) << ' '
     << std::setw(10) << long(totalTime()) << ' '
     << itsFuncName << std::endl;
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

void Util::Prof::printAllProfData(STD_IO::ostream& os) throw()
{
  os.exceptions(STD_IO::ios::goodbit);

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
      if (n > 0)
        {
          for (unsigned int i=0; i+1 < n; ++i)
            {
              STD_IO::cerr << "|   ";
            }
          STD_IO::cerr << "|-->> ";
        }

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
      if (n > 0)
        {
          for (unsigned int i=0; i+1 < n; ++i)
            {
              STD_IO::cerr << "|   ";
            }
          STD_IO::cerr << "| <<--";
        }

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
