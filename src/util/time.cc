///////////////////////////////////////////////////////////////////////
//
// time.cc
//
// Copyright (c) 2002-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Thu Nov  7 16:58:26 2002
// written: Wed Mar 19 17:58:51 2003
// $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef TIME_CC_DEFINED
#define TIME_CC_DEFINED

#include "util/time.h"

#include <sys/resource.h>

Util::Time Util::Time::wallClockNow() throw()
{
  Util::Time t;
  gettimeofday(&t.itsTimeVal, /* timezone */ 0);
  return t;
}

Util::Time Util::Time::rusageNow() throw()
{
  rusage ru;
  getrusage(RUSAGE_SELF, &ru);
  return Util::Time(ru.ru_utime);
}

static const char vcid_time_cc[] = "$Header$";
#endif // !TIME_CC_DEFINED
