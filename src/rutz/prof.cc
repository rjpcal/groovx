/** @file rutz/prof.cc class for accumulating profiling information */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Thu Jun 30 14:47:13 2005
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [https://github.com/rjpcal/groovx]
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

#include "rutz/prof.h"

#include "rutz/abort.h"
#include "rutz/mutex.h"
#include "rutz/staticstack.h"

#include <algorithm> // for std::stable_sort()
#include <cstdio>
#include <functional>
#include <iomanip>
#include <new> // for std::nothrow
#include <ostream>
#include <string>

namespace
{
  //
  // data and thread info for the profile output file
  //

  bool            g_pdata_print_at_exit = false;
  std::string     g_pdata_fname = "prof.out";
  FILE*           g_pdata_file = nullptr;
  std::once_flag  g_pdata_file_once;
  std::mutex      g_pdata_mutex;

  void open_pdata_file()
  {
    if (g_pdata_fname.length() > 0)
      g_pdata_file = fopen(g_pdata_fname.c_str(), "w");

    if (g_pdata_file == nullptr)
      {
        fprintf(stderr,
                "couldn't open profile file '%s' for writing\n",
                g_pdata_fname.c_str());
      }
  }

  //
  // data and thread info for a global list of all rutz::prof objects
  //

  typedef rutz::static_stack<rutz::prof*, 2048> prof_list;

  prof_list*      g_prof_list = nullptr;
  std::once_flag  g_prof_list_once;
  std::mutex      g_prof_list_mutex;

  void initialize_prof_list()
  {
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

    g_prof_list = new (std::nothrow) prof_list;

    if (g_prof_list == nullptr)
      GVX_ABORT("memory allocation failed");
  }

  prof_list& all_profs() noexcept
  {
    std::call_once(g_prof_list_once, initialize_prof_list);

    return *g_prof_list;
  }

  //
  // data and thread info for a global start time
  //

  rutz::time     g_start;
  std::once_flag g_start_once;

  void initialize_start_time()
  {
    g_start = rutz::prof::get_now_time(rutz::prof::get_timing_mode());
  }
}

///////////////////////////////////////////////////////////////////////
//
// rutz::prof member definitions
//
///////////////////////////////////////////////////////////////////////

rutz::prof::timing_mode rutz::prof::s_timing_mode = rutz::prof::timing_mode::RUSAGE;

rutz::prof::prof(const char* s, const char* fname, int lineno)  noexcept:
  m_context_name(s),
  m_src_file_name(fname),
  m_src_line_no(lineno)
{
  reset();

  {
    GVX_MUTEX_LOCK(g_prof_list_mutex);
    all_profs().push(this);
  }

  std::call_once(g_start_once, initialize_start_time);
}

rutz::prof::~prof() noexcept
{
  if (g_pdata_print_at_exit)
    {
      std::call_once(g_pdata_file_once, open_pdata_file);

      GVX_MUTEX_LOCK(g_pdata_mutex);

      if (g_pdata_file != nullptr)
        {
          print_prof_data(g_pdata_file);
        }
    }
}

void rutz::prof::reset() noexcept
{
  m_call_count = 0;
  m_total_time.reset();
  m_children_time.reset();
}

unsigned int rutz::prof::count() const noexcept
{
  return m_call_count;
}

void rutz::prof::add_time(const rutz::time& t) noexcept
{
  m_total_time += t;
  ++m_call_count;
}

void rutz::prof::add_child_time(const rutz::time& t) noexcept
{
  m_children_time += t;
}

const char* rutz::prof::context_name() const noexcept
{
  return m_context_name;
}

const char* rutz::prof::src_file_name() const noexcept
{
  return m_src_file_name;
}

int rutz::prof::src_line_no() const noexcept
{
  return m_src_line_no;
}

double rutz::prof::total_time() const noexcept
{
  return (m_call_count > 0)
    ? m_total_time.usec()
    : 0.0;
}

double rutz::prof::self_time() const noexcept
{
  return (m_call_count > 0)
    ? m_total_time.usec() - m_children_time.usec()
    : 0.0;
}

double rutz::prof::avg_self_time() const noexcept
{
  return m_call_count > 0 ? (total_time() / m_call_count) : 0.0;
}

void rutz::prof::print_prof_data(FILE* file) const noexcept
{
  if (file == nullptr)
    GVX_ABORT("FILE* was null");

  const double total_elapsed_usec =
    (rutz::prof::get_now_time(s_timing_mode) - g_start).usec();

  // Don't try to convert the double values to long or int, because
  // we're likely to overflow and potentially cause a floating-point
  // exception.
  fprintf(file, "%10.0f %6u %10.0f %4.1f%% %10.0f %4.1f%% %s\n",
          avg_self_time(), count(),
          self_time(), (100.0 * self_time()) / total_elapsed_usec,
          total_time(), (100.0 * total_time()) / total_elapsed_usec,
          m_context_name);
}

void rutz::prof::print_prof_data(std::ostream& os) const noexcept
{
  os.exceptions(std::ios::goodbit);

  const double total_elapsed_usec =
    (rutz::prof::get_now_time(s_timing_mode) - g_start).usec();

  // Don't try to convert the double values to long or int, because
  // we're likely to overflow and potentially cause a floating-point
  // exception.
  os << std::fixed
     << std::setw(10) << std::setprecision(0) << avg_self_time() << ' '
     << std::setw(6) << count() << ' '
     << std::setw(10) << std::setprecision(0) << self_time() << ' '
     << std::setw(4) << std::setprecision(1)
     << (100.0 * self_time()) / total_elapsed_usec << "% "
     << std::setw(10) << std::setprecision(0) << total_time() << ' '
     << std::setw(4) << std::setprecision(1)
     << (100.0 * total_time()) / total_elapsed_usec << "% "
     << m_context_name << '\n';
}

void rutz::prof::print_at_exit(bool yes_or_no) noexcept
{
  g_pdata_print_at_exit = yes_or_no;
}

void rutz::prof::prof_summary_file_name(const char* fname)
{
  GVX_MUTEX_LOCK(g_pdata_mutex);

  if (fname == nullptr)
    fname = "";

  g_pdata_fname = fname;
}

void rutz::prof::reset_all_prof_data() noexcept
{
  GVX_MUTEX_LOCK(g_prof_list_mutex);
  std::for_each(all_profs().begin(), all_profs().end(),
                std::mem_fun(&rutz::prof::reset));
}

namespace
{
  // comparison function for use with std::stable_sort()
  bool compare_total_time(rutz::prof* p1, rutz::prof* p2) noexcept
  {
    return p1->total_time() < p2->total_time();
  }
}

void rutz::prof::print_all_prof_data(FILE* file) noexcept
{
  GVX_MUTEX_LOCK(g_prof_list_mutex);
  std::stable_sort(all_profs().begin(), all_profs().end(),
                   compare_total_time);

  for (unsigned int i = 0; i < all_profs().size(); ++i)
    {
      if (all_profs()[i]->count() > 0)
        all_profs()[i]->print_prof_data(file);
    }
}

void rutz::prof::print_all_prof_data(std::ostream& os) noexcept
{
  GVX_MUTEX_LOCK(g_prof_list_mutex);
  std::stable_sort(all_profs().begin(), all_profs().end(),
                   compare_total_time);

  for (unsigned int i = 0; i < all_profs().size(); ++i)
    {
      if (all_profs()[i]->count() > 0)
        all_profs()[i]->print_prof_data(os);
    }
}
