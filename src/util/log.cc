///////////////////////////////////////////////////////////////////////
//
// log.cc
//
// Copyright (c) 2001-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Wed Jun 20 17:49:28 2001
// written: Wed Mar 19 12:45:39 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef LOG_CC_DEFINED
#define LOG_CC_DEFINED

#include "util/log.h"

#include "util/stopwatch.h"
#include "util/strings.h"
#include "util/minivec.h"

#include <algorithm>
#include <functional>
#include <iomanip>
#include <iostream>

#include "util/trace.h"

namespace
{
  StopWatch theTimer;

  struct ScopeInfo
  {
    ScopeInfo(const fstring& name) : itsName(name), itsTimer() {}

    fstring itsName;
    StopWatch itsTimer;

    void print(const timeval* now) const
    {
      DOTRACE("ScopeInfo::print");
      std::cerr << itsName << " @ ";

      std::cerr.setf(std::ios::showpoint | std::ios::fixed);

      std::cerr << std::setprecision(2)
                << itsTimer.elapsedMsec(*now) << " / ";
    }
  };

  minivec<ScopeInfo> scopes;

  template <class Str>
  inline void logImpl(Str msg)
  {
    timeval now; gettimeofday(&now, 0);

    std::for_each(scopes.begin(), scopes.end(),
                  std::bind2nd(std::mem_fun_ref(&ScopeInfo::print), &now));

    std::cerr << msg << std::endl;
  }
}

void Util::Log::reset()
{
DOTRACE("Util::Log::reset");
  scopes.clear();
}

void Util::Log::addScope(const fstring& name)
{
DOTRACE("Util::Log::addScope");
  scopes.push_back(ScopeInfo(name));
}

void Util::Log::removeScope(const fstring& name)
{
DOTRACE("Util::Log::removeScope");
  for (int i = int(scopes.size()) - 1; i > 0; --i)
    {
      if (scopes.at(i).itsName == name)
        {
          scopes.erase(scopes.begin() + i);
        }
    }
}

void Util::log(const char* msg)
{
DOTRACE("Util::log");
  logImpl(msg);
}

void Util::log(const fstring& msg)
{
DOTRACE("Util::log");
  logImpl(msg);
}

static const char vcid_log_cc[] = "$Header$";
#endif // !LOG_CC_DEFINED
