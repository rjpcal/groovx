///////////////////////////////////////////////////////////////////////
//
// time.cc
//
// Copyright (c) 2002-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Thu Nov  7 16:58:26 2002
// written: Wed Mar 19 12:45:35 2003
// $Id$
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
