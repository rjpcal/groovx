///////////////////////////////////////////////////////////////////////
//
// backtrace.cc
//
// Copyright (c) 2004-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Wed Oct 13 16:33:19 2004
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

#ifndef BACKTRACE_CC_DEFINED
#define BACKTRACE_CC_DEFINED

#include "backtrace.h"

#include "util/fstring.h"
#include "util/staticstack.h"

#include <cmath> // for log10()
#include <cstdio> // for snprintf(), fprintf()
#include <new> // for std::nothrow
#include <ostream>

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER

///////////////////////////////////////////////////////////////////////
//
// rutz::backtrace member definitions
//
///////////////////////////////////////////////////////////////////////

struct rutz::backtrace::impl
{
  rutz::static_stack<rutz::prof*, 256> vec;
};

rutz::backtrace::backtrace() throw() :
  rep(new (std::nothrow) impl)
{
  if (rep == 0)
    PANIC("memory allocation failed");
}

rutz::backtrace::backtrace(const backtrace& other) throw() :
  rep(new (std::nothrow) impl(*other.rep))
{
  if (rep == 0)
    PANIC("memory allocation failed");
}

rutz::backtrace& rutz::backtrace::operator=(const backtrace& other) throw()
{
  *rep = *other.rep;
  return *this;
}

rutz::backtrace::~backtrace() throw()
{
  delete rep;
}

rutz::backtrace& rutz::backtrace::current() throw()
{
  static rutz::backtrace* ptr = 0;
  if (ptr == 0)
    {
      ptr = new (std::nothrow) rutz::backtrace;

      if (ptr == 0)
        PANIC("memory allocation failed");
    }
  return *ptr;
}

bool rutz::backtrace::push(rutz::prof* p) throw()
{
  return rep->vec.push(p);
}

void rutz::backtrace::pop() throw()
{
  rep->vec.pop();
}

unsigned int rutz::backtrace::size() const throw()
{
  return rep->vec.size();
}

rutz::prof* rutz::backtrace::top() const throw()
{
  return rep->vec.top();
}

rutz::prof* rutz::backtrace::at(unsigned int i) const throw()
{
  return rep->vec.at(i);
}

void rutz::backtrace::print() const throw()
{
  fprintf(stderr, "stack trace:\n");

  const unsigned int end = size();

  unsigned int i = 0;
  unsigned int ri = end-1;

  for (; i < end; ++i, --ri)
    {
      fprintf(stderr, "\t[%d] %s\n", int(i),
              rep->vec.at(ri)->context_name());
    }
}

void rutz::backtrace::print(std::ostream& os) const throw()
{
  os.exceptions(std::ios::goodbit);

  os << "stack trace:\n";

  const unsigned int end = size();

  unsigned int i = 0;
  unsigned int ri = end-1;

  for (; i < end; ++i, --ri)
    {
      os << "\t[" << i << "] "
         << rep->vec.at(ri)->context_name() << '\n';
    }

  os << std::flush;
}

rutz::fstring rutz::backtrace::format() const
{
  if (rep->vec.size() == 0) return rutz::fstring();

  rutz::fstring result;

  const int LINELEN = 256;
  char line[LINELEN];

  const int width = int(log10(rep->vec.size()-1) + 1.0);

  for (unsigned int i = rep->vec.size(); i > 0; --i)
    {
      snprintf(&line[0], LINELEN, "[%*d] %-35s (%s:%d)\n",
               width,
               rep->vec.size() - i,
               rep->vec[i-1]->context_name(),
               rep->vec[i-1]->src_file_name(),
               rep->vec[i-1]->src_line_no());

      result.append(&line[0]);
    }

  return result;
}

static const char vcid_backtrace_cc[] = "$Id$ $URL$";
#endif // !BACKTRACE_CC_DEFINED
