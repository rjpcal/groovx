///////////////////////////////////////////////////////////////////////
//
// trace.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jan  4 08:00:00 1999
// written: Wed Jul 31 17:16:21 2002
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
  Prof(const char* s);
  ~Prof();

  /// Reset the call count and elapsed time to zero.
  void reset();

  /** Return the number of calls that have been made since the last
      reset(). */
  int count() const;

  void add(timeval t);

  const char* name() const;

  /** Return the total elapsed time in microseconds since the last
      reset(). */
  double totalTime() const;

  /** Return the per-call average of the elapsed time in microseconds since
      the last reset(). */
  double avgTime() const;

  void printProfData(STD_IO::ostream& os) const;

  /** Indicate whether profiling information should be written to the profile
      file at program exit time. */
  static void printAtExit(bool yes_or_no);

  static void resetAllProfData();

  static void printAllProfData(STD_IO::ostream& os);

private:
  Prof(const Prof&);
  Prof& operator=(const Prof&);

  const char* itsFuncName;
  int itsCallCount;
  timeval itsTotalTime;
};

/// Times and traces execution in and out of a lexical scope.
class Util::Trace
{
public:
  enum Mode { RUN, STEP };

  static void setMode(Mode new_mode);
  static Mode getMode();

  static bool getGlobalTrace();
  static void setGlobalTrace(bool on_off);

  static unsigned int getMaxLevel();
  static void setMaxLevel(unsigned int lev);

  Trace(Prof& p, bool useMsg);
  ~Trace();

private:
  void printIn();
  void printOut();

  Prof& prof;
  timeval start, finish, elapsed;
  const bool giveTraceMsg;
};

/// Represents an instantaneous state of the call stack.
class Util::BackTrace
{
public:
  /// Default construct an empty call stack.
  BackTrace();

  /// Copy constructor.
  BackTrace(const BackTrace& other);

  /// Destructor.
  ~BackTrace() throw();

  /// Access the current call stack.
  static BackTrace& current();

  /// Push a new element onto the call stack.
  void push(Util::Prof* p);

  /// Pop the most recent element off of the call stack.
  void pop() throw();

  /// Get the number of elements in the call stack.
  unsigned int size() const throw();

  /// Will return a null pointer if i is out of range.
  Util::Prof* at(unsigned int i) const throw();

  /// Shorthand for at(i).
  Util::Prof* operator[](unsigned int i) const throw() { return at(i); }

  /// Print the call stack on stderr.
  void print() const;

  /// Print the call stack to the given stream.
  void print(STD_IO::ostream& os) const;

private:
  BackTrace& operator=(const BackTrace&);

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
