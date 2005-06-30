///////////////////////////////////////////////////////////////////////
//
// backtrace.cc
//
// Copyright (c) 2004-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Wed Oct 13 16:33:19 2004
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

#ifndef GROOVX_RUTZ_BACKTRACE_CC_UTC20050626084019_DEFINED
#define GROOVX_RUTZ_BACKTRACE_CC_UTC20050626084019_DEFINED

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

rutz::backtrace::backtrace() throw() :
  m_vec()
{}

rutz::backtrace::backtrace(const backtrace& other) throw() :
  m_vec(other.m_vec)
{}

rutz::backtrace& rutz::backtrace::operator=(const backtrace& other) throw()
{
  m_vec = other.m_vec;
  return *this;
}

rutz::backtrace::~backtrace() throw()
{}

rutz::backtrace& rutz::backtrace::current() throw()
{
  static rutz::backtrace* ptr = 0;

  // Q: Why do a dynamic allocation here instead of just having a
  // static object?

  // A: With a static object, we could potentially run in to trouble
  // during program exit, if somebody's destructor called
  // backtrace::current() after the local static object's destructor
  // (i.e., ~backtrace()) had itself already been run. On the other
  // hand, with a dynamically-allocated object, we can just let the
  // memory dangle (it's not really a memory "leak" since the amount
  // of memory is finite and bounded), so the object will never become
  // invalid, even during program shutdown.
  if (ptr == 0)
    {
      ptr = new (std::nothrow) rutz::backtrace;

      if (ptr == 0)
        GVX_ABORT("memory allocation failed");
    }
  return *ptr;
}

bool rutz::backtrace::push(rutz::prof* p) throw()
{
  return m_vec.push(p);
}

void rutz::backtrace::pop() throw()
{
  m_vec.pop();
}

unsigned int rutz::backtrace::size() const throw()
{
  return m_vec.size();
}

rutz::prof* rutz::backtrace::top() const throw()
{
  return m_vec.top();
}

rutz::prof* rutz::backtrace::at(unsigned int i) const throw()
{
  return m_vec.at(i);
}

void rutz::backtrace::print() const throw()
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

void rutz::backtrace::print(std::ostream& os) const throw()
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

static const char vcid_groovx_rutz_backtrace_cc_utc20050626084019[] = "$Id$ $HeadURL$";
#endif // !GROOVX_RUTZ_BACKTRACE_CC_UTC20050626084019_DEFINED
