///////////////////////////////////////////////////////////////////////
//
// trace.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jan  4 08:00:00 1999
// written: Thu Sep 12 15:12:01 2002
// $Id$
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
#include <sys/resource.h>

#include "util/debug.h"

#define TRACE_WALL_CLOCK_TIME
//  #define TRACE_CPU_TIME

template <typename T, unsigned int N>
class static_stack
{
public:
  static_stack() throw() : vec(), top(0) {}

  static_stack(const static_stack& other) throw() :
    vec(), top(0)
  {
    *this = other;
  }

  static_stack& operator=(const static_stack& other) throw()
  {
    for (unsigned int i = 0; i < other.top; ++i)
      {
        vec[i] = other.vec[i];
      }

    return *this;
  }

  unsigned int size() const throw() { return top; }

  static unsigned int capacity() throw() { return N; }

  bool push(T p) throw()
  {
    if (top >= N)
      return false;

    vec[top++] = p;
    return true;
  }

  void pop() throw()
  {
    AbortIf(top == 0);
    --top;
  }

  T at(unsigned int i) const throw()
  {
    return (i < top) ? vec[i] : 0;
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
  unsigned int top;
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
  inline void getTime(timeval& result) throw()
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
  itsFuncName(s), itsCallCount(0), itsTotalTime()
{
  itsTotalTime.tv_sec = 0;
  itsTotalTime.tv_usec = 0;

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
  itsTotalTime.tv_sec = 0;
  itsTotalTime.tv_usec = 0;
}

int Util::Prof::count() const throw()
{
  return itsCallCount;
}

void Util::Prof::add(timeval t) throw()
{
  itsTotalTime.tv_sec += t.tv_sec;
  itsTotalTime.tv_usec += t.tv_usec;
  // avoid overflow
  while (itsTotalTime.tv_usec >= 1000000)
    {
      ++(itsTotalTime.tv_sec);
      itsTotalTime.tv_usec -= 1000000;
    }
  // avoid underflow
  while (itsTotalTime.tv_usec <= -1000000)
    {
      --(itsTotalTime.tv_sec);
      itsTotalTime.tv_usec += 1000000;
    }
  ++itsCallCount;
}

const char* Util::Prof::name() const throw()
{
  return itsFuncName;
}

double Util::Prof::totalTime() const throw()
{
  return (double(itsTotalTime.tv_sec)*1000000 + itsTotalTime.tv_usec);
}

double Util::Prof::avgTime() const throw()
{
  return itsCallCount > 0 ? (totalTime() / itsCallCount) : 0.0;
}

void Util::Prof::printProfData(std::ostream& os) const throw()
{
  os.exceptions(STD_IO::ios::goodbit);

  os << std::setw(14) << long(avgTime()) << '\t'
     << std::setw(5) << count() << '\t'
     << std::setw(14) << long(totalTime()) << '\t'
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
  prof(p),
  start(),
  giveTraceMsg(useMsg),
  shouldPop(true)
{
  if (GLOBAL_TRACE || giveTraceMsg)
    printIn();

  shouldPop = Util::BackTrace::current().push(&p);

  getTime(start);
}

Util::Trace::~Trace() throw()
{
  timeval finish, elapsed;
  getTime(finish);
  elapsed.tv_sec = finish.tv_sec - start.tv_sec;
  elapsed.tv_usec = finish.tv_usec - start.tv_usec;
  prof.add(elapsed);

  if (shouldPop)
    Util::BackTrace::current().pop();

  if (GLOBAL_TRACE || giveTraceMsg)
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

void Util::Trace::printOut() throw()
{
  if (Util::BackTrace::current().size() == 0)
    STD_IO::cerr << '\n';
}

static const char vcid_trace_cc[] = "$Header$";
#endif // TRACE_CC_DEFINED
