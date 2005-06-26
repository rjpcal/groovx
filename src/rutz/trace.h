///////////////////////////////////////////////////////////////////////
//
// trace.h
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Mon Jan  4 08:00:00 1999
// commit: $Id$
// $HeadURL$
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

#ifndef GROOVX_RUTZ_TRACE_H_UTC20050626084019_DEFINED
#define GROOVX_RUTZ_TRACE_H_UTC20050626084019_DEFINED

// This file defines two classes and several macros that can be used
// to achieve function profiling and tracing. The basic idea is that
// for each function for which profiling is enabled, a static
// rutz::prof object is created. This object maintains the call count
// and total elapsed time for that function. The job of measuring and
// recording such information falls to the rutz::trace class. An
// automatic object of the rutz::trace class is constructed on entry
// to a function, and it is destructed just prior to function exit. If
// LOCAL_TRACE is defined, the rutz::trace object will emit "entering"
// and "leaving" messages as it is constructed and destructed,
// respectively. In any case, the rutz::trace object takes care of
// teling the static rutz::prof object to 1) increment its counter,
// and 2) record the elapsed time.
//
// The behavior of the control macros are as follows:
//
// 1) if LOCAL_PROF is defined, profiling will always occur
// 2) if LOCAL_TRACE is defined, profiling AND tracing will always occur
// 3) if TRACE is defined, profiling AND tracing will occur, EXCEPT:
// 4) if NO_TRACE is defined, TRACE is ignored

#include "rutz/time.h"

#include <iosfwd>

namespace rutz
{
  class prof;
  class trace;
}

/// Accumulates profiling information for a given execution context.
class rutz::prof
{
public:
  prof(const char* s, const char* fname, int lineno) throw();
  ~prof() throw();

  /// Reset the call count and elapsed time to zero.
  void reset() throw();

  /// Returns the number of calls since the last reset().
  unsigned int count() const throw();

  void add_time(const rutz::time& t) throw();

  void add_child_time(const rutz::time& t) throw();

  const char* context_name() const throw();

  const char* src_file_name() const throw();

  int src_line_no() const throw();

  /// Get the total elapsed time in microsecs since the last reset().
  double total_time() const throw();

  /// Get the total self time in microsecs since the last reset().
  double self_time() const throw();

  /// Get the per-call average self time in microsecs since the last reset().
  double avg_self_time() const throw();

  /// Print this object's info to the given file.
  void print_prof_data(FILE* f) const throw();

  /// Print this object's info to the given stream.
  void print_prof_data(std::ostream& os) const throw();

  /// Whether to write a profiling summary file when the program exits.
  static void print_at_exit(bool yes_or_no) throw();

  /// Reset all call counts and elapsed times to zero.
  static void reset_all_prof_data() throw();

  /// Print all profile data to the given file.
  static void print_all_prof_data(FILE* f) throw();

  /// Print all profile data to the given stream.
  static void print_all_prof_data(std::ostream& os) throw();

private:
  prof(const prof&) throw();
  prof& operator=(const prof&) throw();

  const char*  const m_context_name;
  const char*  const m_src_file_name;
  int          const m_src_line_no;
  unsigned int       m_call_count;
  rutz::time         m_total_time;
  rutz::time         m_children_time;
};

/// Times and traces execution in and out of a lexical scope.
class rutz::trace
{
public:
  enum mode { RUN, STEP };

  static void set_trace_mode(mode new_mode) throw();
  static mode get_trace_mode() throw();

  static bool get_global_trace() throw();
  static void set_global_trace(bool on_off) throw();

  static unsigned int get_max_level() throw();
  static void set_max_level(unsigned int lev) throw();

  trace(prof& p, bool use_msg) throw();
  ~trace() throw();

private:
  void print_in() throw();
  void print_out() throw();

  prof&      m_prof;
  rutz::time m_start;
  const bool m_should_print_msg;
  bool       m_should_pop;
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
#  define DOTRACE(x) static rutz::prof P_x_(x, __FILE__, __LINE__); \
                     rutz::trace T_x_(P_x_, DYNAMIC_TRACE_EXPR);
#else
#  define DOTRACE(x) {}
#endif

static const char vcid_groovx_rutz_trace_h_utc20050626084019[] = "$Id$ $HeadURL$";
#endif // !GROOVX_RUTZ_TRACE_H_UTC20050626084019_DEFINED
