///////////////////////////////////////////////////////////////////////
//
// trace.cc
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

#ifndef TRACE_CC_DEFINED
#define TRACE_CC_DEFINED

#define LOCAL_PROF
#include "util/trace.h"

#include "util/backtrace.h"
#include "util/staticstack.h"

#include <algorithm>
#include <cstdio>
#include <fstream>
#include <functional>
#include <iostream>
#include <iomanip>
#include <new> // for std::nothrow

#include "util/debug.h"
DBG_REGISTER

///////////////////////////////////////////////////////////////////////
//
// File-scope helper functions and data
//
///////////////////////////////////////////////////////////////////////

#define TRACE_WALL_CLOCK_TIME
//#define TRACE_CPU_TIME

namespace
{
  inline rutz::time get_now_time() throw()
  {
#if defined(TRACE_WALL_CLOCK_TIME)
    return rutz::time::wall_clock_now();
#elif defined(TRACE_CPU_TIME)
    return rutz::time::user_rusage() + rutz::time::sys_rusage();
#else
#  error must define either TRACE_WALL_CLOCK_TIME or TRACE_CPU_TIME
#endif
  }

  unsigned int MAX_TRACE_LEVEL = 20;

  bool GLOBAL_TRACE = false;

  const char* PDATA_FILE = "prof.out";

  bool PRINT_AT_EXIT = true;

  void wait_on_step() throw()
  {
    static char buf[256];

    STD_IO::cerr << "?" << STD_IO::flush;
    STD_IO::cin.getline(&buf[0], 256);

    switch (buf[0])
      {
      case 'r':
      case 'R':
        rutz::trace::set_trace_mode(rutz::trace::RUN);
        break;
      default:
        break;
      }
  }

  typedef rutz::static_stack<rutz::prof*, 2048> prof_list;

  prof_list& all_profs() throw()
  {
    static prof_list* ptr = 0;
    if (ptr == 0)
      {
        ptr = new (std::nothrow) prof_list;

        if (ptr == 0)
          PANIC("memory allocation failed");
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
              fprintf(stderr, "couldn't open profile file for writing\n");
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
  return (m_call_count > 0) ?
    m_total_time.usec()
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
  ASSERT(file != 0);

  fprintf(file, "%10ld %6u %10ld %10ld %s\n",
          long(avg_self_time()), count(),
          long(self_time()), long(total_time()),
          m_context_name);
}

void rutz::prof::print_prof_data(std::ostream& os) const throw()
{
  os.exceptions(STD_IO::ios::goodbit);

  os << std::setw(10) << long(avg_self_time()) << ' '
     << std::setw(6) << count() << ' '
     << std::setw(10) << long(self_time()) << ' '
     << std::setw(10) << long(total_time()) << ' '
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

void rutz::prof::print_all_prof_data(STD_IO::ostream& os) throw()
{
  std::stable_sort(all_profs().begin(), all_profs().end(),
                   compare_total_time);

  for (unsigned int i = 0; i < all_profs().size(); ++i)
    {
      if (all_profs()[i]->count() > 0)
        all_profs()[i]->print_prof_data(os);
    }
}

///////////////////////////////////////////////////////////////////////
//
// rutz::trace member definitions
//
///////////////////////////////////////////////////////////////////////

namespace
{
  rutz::trace::mode current_trace_mode = rutz::trace::RUN;
}

void rutz::trace::set_trace_mode(mode new_mode) throw()
{
  current_trace_mode = new_mode;
}

rutz::trace::mode rutz::trace::get_trace_mode() throw()
{
  return current_trace_mode;
}

bool rutz::trace::get_global_trace() throw()
{
  return GLOBAL_TRACE;
}

void rutz::trace::set_global_trace(bool on_off) throw()
{
  GLOBAL_TRACE = on_off;
}

unsigned int rutz::trace::get_max_level() throw()
{
  return MAX_TRACE_LEVEL;
}

void rutz::trace::set_max_level(unsigned int lev) throw()
{
  MAX_TRACE_LEVEL = lev;
}

rutz::trace::trace(prof& p, bool useMsg) throw() :
  m_prof(p),
  m_start(),
  m_should_print_msg(useMsg),
  m_should_pop(true)
{
  if (GLOBAL_TRACE || m_should_print_msg)
    print_in();

  m_should_pop = rutz::backtrace::current().push(&p);

  m_start = get_now_time();
}

rutz::trace::~trace() throw()
{
  rutz::time finish = get_now_time();
  rutz::time elapsed = finish - m_start;
  m_prof.add_time(elapsed);

  if (m_should_pop)
    {
      rutz::backtrace& c = rutz::backtrace::current();
      c.pop();
      rutz::prof* parent = c.top();
      if (parent != 0)
        parent->add_child_time(elapsed);
    }

  if (GLOBAL_TRACE || m_should_print_msg)
    print_out();
}

void rutz::trace::print_in() throw()
{
  const unsigned int n = rutz::backtrace::current().size();

  if (n < MAX_TRACE_LEVEL)
    {
      for (unsigned int i = 0; i < n; ++i)
        {
          STD_IO::cerr << "|   ";
        }
      // Test whether n has 1, 2, or >3 digits
      if (n < 10)
        STD_IO::cerr << n << "-->> ";
      else if (n < 100)
        STD_IO::cerr << n << "->> ";
      else
        STD_IO::cerr << n << ">> ";

      STD_IO::cerr << m_prof.context_name();

      if (rutz::trace::get_trace_mode() == STEP)
        {
          wait_on_step();
        }
      else
        {
          STD_IO::cerr << '\n';
        }
    }
}

void rutz::trace::print_out() throw()
{
  const unsigned int n = rutz::backtrace::current().size();

  if (n < MAX_TRACE_LEVEL)
    {
      for (unsigned int i = 0; i < n; ++i)
        {
          STD_IO::cerr << "|   ";
        }
      STD_IO::cerr << "| <<--";

      STD_IO::cerr << m_prof.context_name();

      if (rutz::trace::get_trace_mode() == STEP)
        {
          wait_on_step();
        }
      else
        {
          STD_IO::cerr << '\n';
        }
    }

  if (rutz::backtrace::current().size() == 0)
    STD_IO::cerr << '\n';
}

static const char vcid_trace_cc[] = "$Header$";
#endif // TRACE_CC_DEFINED
