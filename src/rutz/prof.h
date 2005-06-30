///////////////////////////////////////////////////////////////////////
//
// prof.h
//
// Copyright (c) 2005-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Thu Jun 30 14:47:11 2005
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

#ifndef GROOVX_RUTZ_PROF_H_UTC20050630214711_DEFINED
#define GROOVX_RUTZ_PROF_H_UTC20050630214711_DEFINED

#include "rutz/time.h"

#include <iosfwd>

namespace rutz
{
  class prof;
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

static const char vcid_groovx_rutz_prof_h_utc20050630214711[] = "$Id$ $HeadURL$";
#endif // !GROOVX_RUTZ_PROF_H_UTC20050630214711DEFINED
