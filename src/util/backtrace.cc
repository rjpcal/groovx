///////////////////////////////////////////////////////////////////////
//
// backtrace.cc
//
// Copyright (c) 2004-2004
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

#include "util/staticstack.h"
#include "util/strings.h"

#include <cmath> // for log10()
#include <cstdio> // for snprintf(), fprintf()
#include <new> // for std::nothrow
#include <ostream>

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER

///////////////////////////////////////////////////////////////////////
//
// Util::BackTrace member definitions
//
///////////////////////////////////////////////////////////////////////

struct Util::BackTrace::Impl
{
  static_stack<rutz::prof*, 256> vec;
};

Util::BackTrace::BackTrace() throw() :
  rep(new (std::nothrow) Impl)
{
  if (rep == 0)
    Panic("memory allocation failed");
}

Util::BackTrace::BackTrace(const BackTrace& other) throw() :
  rep(new (std::nothrow) Impl(*other.rep))
{
  if (rep == 0)
    Panic("memory allocation failed");
}

Util::BackTrace& Util::BackTrace::operator=(const BackTrace& other) throw()
{
  *rep = *other.rep;
  return *this;
}

Util::BackTrace::~BackTrace() throw()
{
  delete rep;
}

Util::BackTrace& Util::BackTrace::current() throw()
{
  static Util::BackTrace* ptr = 0;
  if (ptr == 0)
    {
      ptr = new (std::nothrow) Util::BackTrace;

      if (ptr == 0)
        Panic("memory allocation failed");
    }
  return *ptr;
}

bool Util::BackTrace::push(rutz::prof* p) throw()
{
  return rep->vec.push(p);
}

void Util::BackTrace::pop() throw()
{
  rep->vec.pop();
}

unsigned int Util::BackTrace::size() const throw()
{
  return rep->vec.size();
}

rutz::prof* Util::BackTrace::top() const throw()
{
  return rep->vec.top();
}

rutz::prof* Util::BackTrace::at(unsigned int i) const throw()
{
  return rep->vec.at(i);
}

void Util::BackTrace::print() const throw()
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

void Util::BackTrace::print(STD_IO::ostream& os) const throw()
{
  os.exceptions(STD_IO::ios::goodbit);

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

fstring Util::BackTrace::format() const
{
  if (rep->vec.size() == 0) return fstring();

  fstring result;

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

static const char vcid_backtrace_cc[] = "$Header$";
#endif // !BACKTRACE_CC_DEFINED
