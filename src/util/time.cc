///////////////////////////////////////////////////////////////////////
//
// time.cc
//
// Copyright (c) 2002-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Thu Nov  7 16:58:26 2002
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

#ifndef TIME_CC_DEFINED
#define TIME_CC_DEFINED

#include "util/time.h"

#include "util/fstring.h"

#include <sys/resource.h>
#include <time.h>

rutz::time rutz::time::wall_clock_now() throw()
{
  rutz::time t;
  gettimeofday(&t.m_timeval, /* timezone */ 0);
  return t;
}

rutz::time rutz::time::user_rusage() throw()
{
  rusage ru;
  getrusage(RUSAGE_SELF, &ru);
  return rutz::time(ru.ru_utime);
}

rutz::time rutz::time::sys_rusage() throw()
{
  rusage ru;
  getrusage(RUSAGE_SELF, &ru);
  return rutz::time(ru.ru_stime);
}

rutz::fstring rutz::time::format(const char* formatcode) const
{
  const time_t t = time_t(m_timeval.tv_sec);

  struct tm* tt = localtime(&t);

  char buf[512];

  std::size_t count = strftime(buf, 512, formatcode, tt);

  return rutz::fstring(rutz::char_range(&buf[0], count));
}

static const char vcid_time_cc[] = "$Header$";
#endif // !TIME_CC_DEFINED
