///////////////////////////////////////////////////////////////////////
//
// log.cc
//
// Copyright (c) 2001-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Wed Jun 20 17:49:28 2001
// commit: $Id$
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

#ifndef LOG_CC_DEFINED
#define LOG_CC_DEFINED

#include "util/log.h"

#include "util/object.h"
#include "util/pointers.h"
#include "util/stopwatch.h"
#include "util/strings.h"

#include <algorithm>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <vector>

#include "util/trace.h"

namespace
{
  struct ScopeInfo
  {
    ScopeInfo(const fstring& name) : itsName(name), itsTimer() {}

    fstring itsName;
    StopWatch itsTimer;

    void print(std::ostream& os, const Util::Time& now) const
    {
      DOTRACE("ScopeInfo::print");
      os << itsName << " @ ";

      os.setf(std::ios::showpoint | std::ios::fixed);

      os << std::setprecision(3)
         << itsTimer.elapsed(now).msec() << " | ";
    }
  };

  std::vector<ScopeInfo> scopes;
  shared_ptr<std::ofstream> logFile(0);
  bool copyToStdout = true;

  template <class Str>
  inline void logImpl(std::ostream& os, Str msg)
  {
    const Util::Time now = Util::Time::wallClockNow();

    for (unsigned int i = 0; i < scopes.size(); ++i)
      {
        scopes[i].print(os, now);
      }

    os << msg << std::endl;
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
  for (int i = int(scopes.size()); i > 0; /* decr in loop body */)
    {
      --i;
      if (scopes.at(i).itsName == name)
        {
          scopes.erase(scopes.begin() + i);

          // Return immediately, since this function is intended to remove
          // at most one scope from the stack of scopes.
          return;
        }
    }
}

void Util::Log::addObjScope(const Util::Object& obj)
{
DOTRACE("Util::Log::addObjScope");

  const fstring scopename(obj.uniqueName());

  addScope(scopename);

  log(fstring("entering ", scopename));
}

void Util::Log::removeObjScope(const Util::Object& obj)
{
DOTRACE("Util::Log::removeObjScope");

  const fstring scopename = obj.uniqueName();

  log(fstring("leaving ", scopename));

  removeScope(scopename);
}

void Util::Log::setLogFilename(const fstring& filename)
{
DOTRACE("Util::Log::setLogFilename");

  shared_ptr<std::ofstream> newfile
    (new std::ofstream(filename.c_str(), std::ios::out | std::ios::app));

  if (newfile->is_open() && newfile->good())
    logFile.swap(newfile);
}

void Util::Log::setCopyToStdout(bool shouldcopy)
{
DOTRACE("Util::Log::setCopyToStdout");
  copyToStdout = shouldcopy;
}

void Util::log(const char* msg)
{
DOTRACE("Util::log");
  if (copyToStdout)
    logImpl(std::cout, msg);

  if (logFile.get() != 0)
    logImpl(*logFile, msg);
}

void Util::log(const fstring& msg)
{
DOTRACE("Util::log");
  if (copyToStdout)
    logImpl(std::cout, msg);

  if (logFile.get() != 0)
    logImpl(*logFile, msg);
}

static const char vcid_log_cc[] = "$Header$";
#endif // !LOG_CC_DEFINED
