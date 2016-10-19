/** @file rutz/time.cc user-friendly wrapper around timeval */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Thu Nov  7 16:58:26 2002
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

#include "rutz/time.h"

#include <sys/resource.h>
#include <time.h>

rutz::time rutz::time::wall_clock_now() noexcept
{
  rutz::time t;
  gettimeofday(&t.m_timeval, /* timezone */ 0);
  return t;
}

rutz::time rutz::time::user_rusage() noexcept
{
  rusage ru;
  getrusage(RUSAGE_SELF, &ru);
  return rutz::time(ru.ru_utime);
}

rutz::time rutz::time::sys_rusage() noexcept
{
  rusage ru;
  getrusage(RUSAGE_SELF, &ru);
  return rutz::time(ru.ru_stime);
}
