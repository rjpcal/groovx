///////////////////////////////////////////////////////////////////////
//
// trace.h
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
}

/// Accumulates profiling information for a given execution context.
class Util::Prof
{
public:
  Prof(const char* s, const char* fname, int lineno) throw();
  ~Prof() throw();

  /// Reset the call count and elapsed time to zero.
  void reset() throw();

  /// Returns the number of calls since the last reset().
  unsigned int count() const throw();

  void add(const rutz::time& t) throw();

  void addChildTime(const rutz::time& t) throw();

  const char* name() const throw();

  const char* srcFileName() const throw();

  int srcLineNo() const throw();

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

  const char*  const itsFuncName;
  const char*  const itsSourceFileName;
  int          const itsSourceLineNo;
  unsigned int       itsCallCount;
  rutz::time         itsTotalTime;
  rutz::time         itsChildrenTime;
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
  rutz::time itsStart;
  const bool itsGiveTraceMsg;
  bool itsShouldPop;
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
#  define DOTRACE(x) static Util::Prof P_x_(x, __FILE__, __LINE__); \
                     Util::Trace T_x_(P_x_, DYNAMIC_TRACE_EXPR);
#else
#  define DOTRACE(x) {}
#endif

static const char vcid_trace_h[] = "$Header$";
#endif // !TRACE_H_DEFINED
