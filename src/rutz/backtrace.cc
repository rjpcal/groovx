/** @file rutz/backtrace.cc represents the state of the call stack as
    given by GVX_TRACE statements */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Wed Oct 13 16:33:19 2004
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

#include "rutz/backtrace.h"

#include "rutz/abort.h"
#include "rutz/prof.h"

#include <cstdio>  // for fprintf()
#include <ostream>

///////////////////////////////////////////////////////////////////////
//
// rutz::backtrace member definitions
//
///////////////////////////////////////////////////////////////////////

rutz::backtrace::backtrace() noexcept :
  m_vec()
{}

rutz::backtrace::backtrace(const backtrace& other) noexcept :
  m_vec(other.m_vec)
{}

rutz::backtrace& rutz::backtrace::operator=(const backtrace& other) noexcept
{
  m_vec = other.m_vec;
  return *this;
}

rutz::backtrace::~backtrace() noexcept
{}

rutz::backtrace& rutz::backtrace::current() noexcept
{
  thread_local static rutz::backtrace* current_backtrace = nullptr;

  if (!current_backtrace)
    {
      current_backtrace = new (std::nothrow) rutz::backtrace;
      if (!current_backtrace)
        GVX_ABORT("memory allocation failed");
    }

  return *current_backtrace;
}

bool rutz::backtrace::push(rutz::prof* p) noexcept
{
  return m_vec.push(p);
}

void rutz::backtrace::pop() noexcept
{
  m_vec.pop();
}

unsigned int rutz::backtrace::size() const noexcept
{
  return m_vec.size();
}

rutz::prof* rutz::backtrace::top() const noexcept
{
  return m_vec.top();
}

rutz::prof* rutz::backtrace::at(unsigned int i) const noexcept
{
  return m_vec.at(i);
}

void rutz::backtrace::print() const noexcept
{
  const unsigned int end = size();

  if (end == 0)
    return; // don't print anything if we have an empty backtrace

  fprintf(stderr, "stack trace:\n");

  unsigned int i = 0;
  unsigned int ri = end-1;

  for (; i < end; ++i, --ri)
    {
      fprintf(stderr, "\t[%d] %s\n", int(i),
              m_vec.at(ri)->context_name());
    }
}

void rutz::backtrace::print(std::ostream& os) const noexcept
{
  const unsigned int end = size();

  if (end == 0)
    return; // don't print anything if we have an empty backtrace

  os.exceptions(std::ios::goodbit);

  os << "stack trace:\n";

  unsigned int i = 0;
  unsigned int ri = end-1;

  for (; i < end; ++i, --ri)
    {
      os << "\t[" << i << "] "
         << m_vec.at(ri)->context_name() << '\n';
    }

  os << std::flush;
}
