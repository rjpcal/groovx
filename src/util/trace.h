///////////////////////////////////////////////////////////////////////
//
// trace.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jan  4 08:00:00 1999
// written: Sun Aug 26 08:53:50 2001
// $Id$
//
// This file defines two classes and several macros that can be used
// to achieve function profiling and tracing. The basic idea is that
// for each function for which profiling is enabled, a static Prof
// object is created. This object maintains the call count and total
// elapsed time for that function. The job of measuring and recording
// such information falls to the Trace class. An automatic object of
// the Trace class is constructed on entry to a function, and it is
// destructed just prior to function exit. If LOCAL_TRACE is defined,
// the Trace object will emit "entering" and "leaving" messages as it
// is constructed and destructed, respectively. In any case, the Trace
// object takes care of teling the static Prof object to 1) increment
// its counter, and 2) record the elapsed time.
//
// The behavior of the control macros are as follows:
//
// 1) if LOCAL_PROF is defined, profiling will always occur
// 2) if LOCAL_TRACE is defined, profiling AND tracing will always occur
// 3) if TRACE is defined, profiling AND tracing will occur, EXCEPT:
// 4) if NO_TRACE is defined, TRACE is ignored
//
///////////////////////////////////////////////////////////////////////

#ifndef TRACE_H_DEFINED
#define TRACE_H_DEFINED

#if (defined(TRACE) && !defined(NO_TRACE))
#  ifndef LOCAL_TRACE
#    define LOCAL_TRACE
#  endif
#endif

#if defined(LOCAL_TRACE) || (defined(PROF) && !defined(NO_PROF))
#  ifndef LOCAL_PROF
#    define LOCAL_PROF
#  endif
#endif

#if !defined(DYNAMIC_TRACE_EXPR)
#  if defined(LOCAL_TRACE)
#    define DYNAMIC_TRACE_EXPR true
#  else
#    define DYNAMIC_TRACE_EXPR false
#  endif
#endif


#ifdef PRESTANDARD_IOSTREAMS
class ostream;
#else
#  if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IOSFWD_DEFINED)
#    include <iosfwd>
#    define IOSFWD_DEFINED
#  endif
#endif

#include <sys/time.h>
// struct timeval {
//                unsigned long  tv_sec;   /* seconds since Jan. 1, 1970 */
//                long           tv_usec;  /* and microseconds */
//            };


extern int MAX_TRACE_LEVEL;

extern int TRACE_LEVEL;
const char* const TRACE_TAB = "  ";

namespace Util
{
  class Prof;
  class Trace;
}

class Util::Prof
{
public:
  Prof(const char* s);
  ~Prof();

  int count() const { return callCount; }

  void add(timeval t)
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

  const char* name() const { return funcName; }

  double avgTime() const
    {
      return (double(totalTime.tv_sec)*1000000 + totalTime.tv_usec)
        / callCount;
    }

  void printProfData(STD_IO::ostream& os) const;

  static void resetAllProfData();

  static void printAllProfData(STD_IO::ostream& os);

private:
  Prof(const Prof&);
  Prof& operator=(const Prof&);

  const char* funcName;
  int callCount;
  timeval totalTime;
};

class Util::Trace
{
public:
  enum Mode { RUN, STEP };

  static void setMode(Mode new_mode);
  static Mode getMode();

  static void printStackTrace(); // just print on cerr
  static void printStackTrace(STD_IO::ostream& os);

  Trace(Prof& p, bool useMsg);
  ~Trace();

private:
  void printIn();
  void printOut();

  Prof& prof;
  timeval start, finish, elapsed;
  const bool giveTraceMsg;
};

#ifdef LOCAL_PROF
#  define DOTRACE(x) static Util::Prof P__(x); \
                     Util::Trace T__(P__, DYNAMIC_TRACE_EXPR);
#else
#  define DOTRACE(x) {}
#endif

static const char vcid_trace_h[] = "$Header$";
#endif // !TRACE_H_DEFINED
