///////////////////////////////////////////////////////////////////////
//
// trace.h
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Mon Jan  4 08:00:00 1999
// commit: $Id$
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

#ifndef TRACE_H_DEFINED
#define TRACE_H_DEFINED

// This file defines two classes and several macros that can be used to
// achieve function profiling and tracing. The basic idea is that for each
// function for which profiling is enabled, a static Prof object is
// created. This object maintains the call count and total elapsed time for
// that function. The job of measuring and recording such information falls
// to the Trace class. An automatic object of the Trace class is
// constructed on entry to a function, and it is destructed just prior to
// function exit. If LOCAL_TRACE is defined, the Trace object will emit
// "entering" and "leaving" messages as it is constructed and destructed,
// respectively. In any case, the Trace object takes care of teling the
// static Prof object to 1) increment its counter, and 2) record the
// elapsed time.
//
// The behavior of the control macros are as follows:
//
// 1) if LOCAL_PROF is defined, profiling will always occur
// 2) if LOCAL_TRACE is defined, profiling AND tracing will always occur
// 3) if TRACE is defined, profiling AND tracing will occur, EXCEPT:
// 4) if NO_TRACE is defined, TRACE is ignored

#include "util/time.h"

#ifdef HAVE_IOSFWD
#  include <iosfwd>
#else
class ostream;
#endif

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

  /// Returns the number of calls since the last reset().
  unsigned int count() const throw();

  void add(const Util::Time& t) throw();

  void addChildTime(const Util::Time& t) throw();

  const char* name() const throw();

  /// Returns the total elapsed time in microseconds since the last reset().
  double totalTime() const throw();

  /// Returns the total self time in microseconds since the last reset().
  double selfTime() const throw();

  /// Return the per-call average time in microseconds since the last reset().
  double avgTime() const throw();

  /// Print this object's info to the given file.
  void printProfData(FILE* f) const throw();

  /// Print this object's info to the given stream.
  void printProfData(STD_IO::ostream& os) const throw();

  /// Whether to write a profiling summary file when the program exits.
  static void printAtExit(bool yes_or_no) throw();

  /// Reset all call counts and elapsed times to zero.
  static void resetAllProfData() throw();

  /// Print all profile data to the given file.
  static void printAllProfData(FILE* f) throw();

  /// Print all profile data to the given stream.
  static void printAllProfData(STD_IO::ostream& os) throw();

private:
  Prof(const Prof&) throw();
  Prof& operator=(const Prof&) throw();

  const char* itsFuncName;
  unsigned int itsCallCount;
  Util::Time itsTotalTime;
  Util::Time itsChildrenTime;
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

  Prof& itsProf;
  Util::Time itsStart;
  const bool itsGiveTraceMsg;
  bool itsShouldPop;
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

  /// Get the top stack frame, or null if the backtrace is empty.
  Util::Prof* top() const throw();

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

#ifdef LOCAL_PROF
#  define DOTRACE(x) static Util::Prof P_x_(x); \
                     Util::Trace T_x_(P_x_, DYNAMIC_TRACE_EXPR);
#else
#  define DOTRACE(x) {}
#endif

static const char vcid_trace_h[] = "$Header$";
#endif // !TRACE_H_DEFINED
