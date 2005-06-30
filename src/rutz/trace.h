///////////////////////////////////////////////////////////////////////
//
// trace.h
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Mon Jan  4 08:00:00 1999
// commit: $Id$
// $HeadURL$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
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

// This file defines the rutz::trace class and several macros, which
// along with rutz::prof, can be used to achieve function profiling
// and tracing. The basic idea is that for each function for which
// profiling is enabled, a static rutz::prof object is created. This
// object maintains the call count and total elapsed time for that
// function. The job of measuring and recording such information falls
// to the rutz::trace class. An automatic object of the rutz::trace
// class is constructed on entry to a function, and it is destructed
// just prior to function exit. If GVX_LOCAL_TRACE is defined, the
// rutz::trace object will emit "entering" and "leaving" messages as
// it is constructed and destructed, respectively. In any case, the
// rutz::trace object takes care of teling the static rutz::prof
// object to 1) increment its counter, and 2) record the elapsed time.
//
// The behavior of the control macros are as follows:
//
// 1) if GVX_LOCAL_PROF is defined, profiling will always occur
// 2) if GVX_LOCAL_TRACE is defined, profiling AND tracing will always occur
// 3) if GVX_TRACE is defined, profiling AND tracing will occur, EXCEPT:
// 4) if GVX_NO_TRACE is defined, GVX_TRACE is ignored

#include "rutz/prof.h"
#include "rutz/time.h"

#include <iosfwd>

namespace rutz
{
  class trace;
}

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

#if (defined(GVX_TRACE) && !defined(GVX_NO_TRACE))
#  ifndef GVX_LOCAL_TRACE
#    define GVX_LOCAL_TRACE
#  endif
#endif

#if defined(GVX_LOCAL_TRACE) || (defined(GVX_PROF) && !defined(GVX_NO_PROF))
#  ifndef GVX_LOCAL_PROF
#    define GVX_LOCAL_PROF
#  endif
#endif

#if !defined(GVX_DYNAMIC_TRACE_EXPR)
#  if defined(GVX_LOCAL_TRACE)
#    define GVX_DYNAMIC_TRACE_EXPR true
#  else
#    define GVX_DYNAMIC_TRACE_EXPR false
#  endif
#endif

#ifdef GVX_LOCAL_PROF
#  define GVX_TRACE(x) \
                 static rutz::prof  P_x_  (x,   __FILE__, __LINE__); \
                 rutz::trace        T_x_  (P_x_, GVX_DYNAMIC_TRACE_EXPR);
#else
#  define GVX_TRACE(x) {}
#endif

static const char vcid_groovx_rutz_trace_h_utc20050626084019[] = "$Id$ $HeadURL$";
#endif // !GROOVX_RUTZ_TRACE_H_UTC20050626084019_DEFINED
