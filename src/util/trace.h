///////////////////////////////////////////////////////////////////////
//
// trace.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jan  4 08:00:00 1999
// written: Wed Sep 25 19:00:32 2002
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


#ifdef HAVE_IOSFWD
#  include <iosfwd>
#else
class ostream;
#endif

#include <sys/time.h>
// struct timeval {
//                unsigned long  tv_sec;   /* seconds since Jan. 1, 1970 */
//                long           tv_usec;  /* and microseconds */
//            };


namespace Util
{
  class Prof;
  class Trace;
  class BackTrace;
}

/// Accumulates profiling information for a given execution context.
class Util::Prof
{
public:
  Prof(const char* s) throw();
  ~Prof() throw();

  /// Reset the call count and elapsed time to zero.
  void reset() throw();

  /** Return the number of calls that have been made since the last
      reset(). */
  int count() const throw();

  void add(timeval t) throw();

  const char* name() const throw();

  /** Return the total elapsed time in microseconds since the last
      reset(). */
  double totalTime() const throw();

  /** Return the per-call average of the elapsed time in microseconds since
      the last reset(). */
  double avgTime() const throw();

  void printProfData(STD_IO::ostream& os) const throw();

  /** Indicate whether profiling information should be written to the profile
      file at program exit time. */
  static void printAtExit(bool yes_or_no) throw();

  static void resetAllProfData() throw();

  static void printAllProfData(STD_IO::ostream& os) throw();

private:
  Prof(const Prof&) throw();
  Prof& operator=(const Prof&) throw();

  const char* itsFuncName;
  int itsCallCount;
  timeval itsTotalTime;
};

/// Times and traces execution in and out of a lexical scope.
class Util::Trace
{
public:
  enum Mode { RUN, STEP };

  static void setMode(Mode new_mode) throw();
  static Mode getMode() throw();

  static bool getGlobalTrace() throw();
  static void setGlobalTrace(bool on_off) throw();

  static unsigned int getMaxLevel() throw();
  static void setMaxLevel(unsigned int lev) throw();

  Trace(Prof& p, bool useMsg) throw();
  ~Trace() throw();

private:
  void printIn() throw();
  void printOut() throw();

  Prof& prof;
  timeval start;
  const bool giveTraceMsg;
  bool shouldPop;
};

/// Represents an instantaneous state of the call stack.
class Util::BackTrace
{
public:
  /// Default construct an empty call stack.
  BackTrace() throw();

  /// Copy constructor.
  BackTrace(const BackTrace& other) throw();

  /// Assignment operator.
  BackTrace& operator=(const BackTrace& other) throw();

  /// Destructor.
  ~BackTrace() throw();

  /// Access the current call stack.
  static BackTrace& current() throw();

  /// Push a new element onto the call stack. Returns true if successful.
  bool push(Util::Prof* p) throw();

  /// Pop the most recent element off of the call stack.
  void pop() throw();

  /// Get the number of elements in the call stack.
  unsigned int size() const throw();

  /// Will return a null pointer if i is out of range.
  Util::Prof* at(unsigned int i) const throw();

  /// Shorthand for at(i).
  Util::Prof* operator[](unsigned int i) const throw() { return at(i); }

  /// Print the call stack on stderr.
  void print() const throw();

  /// Print the call stack to the given stream.
  void print(STD_IO::ostream& os) const throw();

private:
  struct Impl;
  Impl* const rep;
};

#ifdef LOCAL_PROF
#  define DOTRACE(x) static Util::Prof P_x_(x); \
                     Util::Trace T_x_(P_x_, DYNAMIC_TRACE_EXPR);
#else
#  define DOTRACE(x) {}
#endif

static const char vcid_trace_h[] = "$Header$";
#endif // !TRACE_H_DEFINED
