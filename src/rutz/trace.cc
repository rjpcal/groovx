/** @file rutz/trace.cc */

///////////////////////////////////////////////////////////////////////
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

#ifndef GROOVX_RUTZ_TRACE_CC_UTC20050626084020_DEFINED
#define GROOVX_RUTZ_TRACE_CC_UTC20050626084020_DEFINED

#include "rutz/trace.h"

#include "rutz/backtrace.h"

#include <iostream>

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

  void wait_on_step() throw()
  {
    static char buf[256];

    std::cerr << "?" << std::flush;
    std::cin.getline(&buf[0], 256);

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
          std::cerr << "|   ";
        }
      // Test whether n has 1, 2, or >3 digits
      if (n < 10)
        std::cerr << n << "-->> ";
      else if (n < 100)
        std::cerr << n << "->> ";
      else
        std::cerr << n << ">> ";

      std::cerr << m_prof.context_name();

      if (rutz::trace::get_trace_mode() == STEP)
        {
          wait_on_step();
        }
      else
        {
          std::cerr << '\n';
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
          std::cerr << "|   ";
        }
      std::cerr << "| <<--";

      std::cerr << m_prof.context_name();

      if (rutz::trace::get_trace_mode() == STEP)
        {
          wait_on_step();
        }
      else
        {
          std::cerr << '\n';
        }
    }

  if (rutz::backtrace::current().size() == 0)
    std::cerr << '\n';
}

static const char vcid_groovx_rutz_trace_cc_utc20050626084020[] = "$Id$ $HeadURL$";
#endif // !GROOVX_RUTZ_TRACE_CC_UTC20050626084020_DEFINED
