///////////////////////////////////////////////////////////////////////
//
// log.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Jun 20 17:49:28 2001
// written: Fri Jan 18 16:07:04 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef LOG_CC_DEFINED
#define LOG_CC_DEFINED

#include "util/log.h"

#include <iostream.h>

namespace
{
  Util::Log* theLog=0;
}

Util::Log& Util::Log::operator<<(char n)
{
  itsOs << n; return *this;
}

Util::Log& Util::Log::operator<<(const char* n)
{
  itsOs << n; return *this;
}

Util::Log& Util::Log::operator<<(int n)
{
  itsOs << n; return *this;
}

Util::Log& Util::Log::operator<<(unsigned int n)
{
  itsOs << n; return *this;
}

Util::Log& Util::Log::operator<<(long n)
{
  itsOs << n; return *this;
}

Util::Log& Util::Log::operator<<(unsigned long n)
{
  itsOs << n; return *this;
}

Util::Log& Util::Log::operator<<(double d)
{
  itsOs << d; return *this;
}

Util::Log& Util::log()
{
  if (theLog == 0)
    {
      theLog = new Util::Log(std::cerr);
    }
  return *theLog;
}

static const char vcid_log_cc[] = "$Header$";
#endif // !LOG_CC_DEFINED
