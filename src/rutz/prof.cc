/** @file rutz/prof.cc class for accumulating profiling information */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2005-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Thu Jun 30 14:47:13 2005
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

#ifndef GROOVX_RUTZ_PROF_CC_UTC20050630214713_DEFINED
#define GROOVX_RUTZ_PROF_CC_UTC20050630214713_DEFINED

#include "rutz/prof.h"

#include "rutz/abort.h"
#include "rutz/staticstack.h"

#include <algorithm> // for std::stable_sort()
#include <cstdio>
#include <functional>
#include <iomanip>
#include <iostream>
#include <new> // for std::nothrow

namespace
{
  const char* PDATA_FILE = "prof.out";

  bool PRINT_AT_EXIT = false;

  typedef rutz::static_stack<rutz::prof*, 2048> prof_list;

  prof_list& all_profs() throw()
  {
    static prof_list* ptr = 0;

    // Q: Why do a dynamic allocation here instead of just having a
    // static object?

    // A: With a static object, we could potentially run in to trouble
    // during program exit, if somebody's destructor called
    // backtrace::current() after the local static object's destructor
    // (i.e., ~backtrace()) had itself already been run. On the other
    // hand, with a dynamically-allocated object, we can just let the
    // memory dangle (it's not really a memory "leak" since the amount
    // of memory is finite and bounded), so the object will never
    // become invalid, even during program shutdown.
    if (ptr == 0)
      {
        ptr = new (std::nothrow) prof_list;

        if (ptr == 0)
          GVX_ABORT("memory allocation failed");
      }
    return *ptr;
  }
}

///////////////////////////////////////////////////////////////////////
//
// rutz::prof member definitions
//
///////////////////////////////////////////////////////////////////////

rutz::prof::prof(const char* s, const char* fname, int lineno)  throw():
  m_context_name(s),
  m_src_file_name(fname),
  m_src_line_no(lineno)
{
  reset();

  all_profs().push(this);
}

rutz::prof::~prof() throw()
{
  if (PRINT_AT_EXIT)
    {
      static FILE* file = 0;
      static bool inited = false;

      if (!inited)
        {
          file = fopen(PDATA_FILE, "w");

          // need this extra state flag since it's possible that the
          // fopen() call above fails, so we can't simply check
          // (file!=0) to see if initialization has already been tried
          inited = true;

          if (file == 0)
            {
              fprintf(stderr,
                      "couldn't open profile file '%s' for writing\n",
                      PDATA_FILE);
            }
        }

      if (file != 0)
        {
          print_prof_data(file);
        }
    }
}

void rutz::prof::reset() throw()
{
  m_call_count = 0;
  m_total_time.reset();
  m_children_time.reset();
}

unsigned int rutz::prof::count() const throw()
{
  return m_call_count;
}

void rutz::prof::add_time(const rutz::time& t) throw()
{
  m_total_time += t;
  ++m_call_count;
}

void rutz::prof::add_child_time(const rutz::time& t) throw()
{
  m_children_time += t;
}

const char* rutz::prof::context_name() const throw()
{
  return m_context_name;
}

const char* rutz::prof::src_file_name() const throw()
{
  return m_src_file_name;
}

int rutz::prof::src_line_no() const throw()
{
  return m_src_line_no;
}

double rutz::prof::total_time() const throw()
{
  return (m_call_count > 0)
    ? m_total_time.usec()
    : 0.0;
}

double rutz::prof::self_time() const throw()
{
  return (m_call_count > 0)
    ? m_total_time.usec() - m_children_time.usec()
    : 0.0;
}

double rutz::prof::avg_self_time() const throw()
{
  return m_call_count > 0 ? (total_time() / m_call_count) : 0.0;
}

void rutz::prof::print_prof_data(FILE* file) const throw()
{
  if (file == 0)
    GVX_ABORT("FILE* was null");

  // Don't try to convert the double values to long or int, because
  // we're likely to overflow and potentially cause a floating-point
  // exception.
  fprintf(file, "%10.0f %6u %10.0f %10.0f %s\n",
          avg_self_time(), count(),
          self_time(), total_time(),
          m_context_name);
}

void rutz::prof::print_prof_data(std::ostream& os) const throw()
{
  os.exceptions(std::ios::goodbit);

  // Don't try to convert the double values to long or int, because
  // we're likely to overflow and potentially cause a floating-point
  // exception.
  os << std::setw(10) << std::setprecision(0) << avg_self_time() << ' '
     << std::setw(6) << count() << ' '
     << std::setw(10) << std::setprecision(0) << self_time() << ' '
     << std::setw(10) << std::setprecision(0) << total_time() << ' '
     << m_context_name << '\n';
}

void rutz::prof::print_at_exit(bool yes_or_no) throw()
{
  PRINT_AT_EXIT = yes_or_no;
}

void rutz::prof::reset_all_prof_data() throw()
{
  std::for_each(all_profs().begin(), all_profs().end(),
                std::mem_fun(&rutz::prof::reset));
}

namespace
{
  // comparison function for use with std::stable_sort()
  bool compare_total_time(rutz::prof* p1, rutz::prof* p2) throw()
  {
    return p1->total_time() < p2->total_time();
  }
}

void rutz::prof::print_all_prof_data(FILE* file) throw()
{
  std::stable_sort(all_profs().begin(), all_profs().end(),
                   compare_total_time);

  for (unsigned int i = 0; i < all_profs().size(); ++i)
    {
      if (all_profs()[i]->count() > 0)
        all_profs()[i]->print_prof_data(file);
    }
}

void rutz::prof::print_all_prof_data(std::ostream& os) throw()
{
  std::stable_sort(all_profs().begin(), all_profs().end(),
                   compare_total_time);

  for (unsigned int i = 0; i < all_profs().size(); ++i)
    {
      if (all_profs()[i]->count() > 0)
        all_profs()[i]->print_prof_data(os);
    }
}

static const char vcid_groovx_rutz_prof_cc_utc20050630214713[] = "$Id$ $HeadURL$";
#endif // !GROOVX_RUTZ_PROF_CC_UTC20050630214713DEFINED
