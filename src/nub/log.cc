///////////////////////////////////////////////////////////////////////
//
// log.cc
//
// Copyright (c) 2001-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Wed Jun 20 17:49:28 2001
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

#ifndef LOG_CC_DEFINED
#define LOG_CC_DEFINED

#include "log.h"

#include "nub/object.h"

#include "util/fstring.h"
#include "util/sharedptr.h"
#include "util/stopwatch.h"

#include <algorithm>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <vector>

#include "util/trace.h"

using rutz::fstring;

namespace
{
  struct ScopeInfo
  {
    ScopeInfo(const fstring& name) : itsName(name), itsTimer() {}

    fstring itsName;
    rutz::stopwatch itsTimer;

    void print(std::ostream& os, const rutz::time& now) const
    {
      DOTRACE("ScopeInfo::print");
      os << itsName << " @ ";

      os.setf(std::ios::showpoint | std::ios::fixed);

      os << std::setprecision(3)
         << itsTimer.elapsed(now).msec() << " | ";
    }
  };

  std::vector<ScopeInfo> scopes;
  rutz::shared_ptr<std::ofstream> logFile;
  bool copyToStdout = true;

  template <class Str>
  inline void logImpl(std::ostream& os, Str msg)
  {
    const rutz::time now = rutz::time::wall_clock_now();

    for (unsigned int i = 0; i < scopes.size(); ++i)
      {
        scopes[i].print(os, now);
      }

    os << msg << std::endl;
  }
}

void Nub::Log::reset()
{
DOTRACE("Nub::Log::reset");
  scopes.clear();

  log(fstring("log reset ", rutz::time::wall_clock_now().format()));
}

void Nub::Log::addScope(const fstring& name)
{
DOTRACE("Nub::Log::addScope");
  scopes.push_back(ScopeInfo(name));
}

void Nub::Log::removeScope(const fstring& name)
{
DOTRACE("Nub::Log::removeScope");
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

void Nub::Log::addObjScope(const Nub::Object& obj)
{
DOTRACE("Nub::Log::addObjScope");

  const fstring scopename(obj.uniqueName());

  addScope(scopename);

  log(fstring("entering ", scopename));
}

void Nub::Log::removeObjScope(const Nub::Object& obj)
{
DOTRACE("Nub::Log::removeObjScope");

  const fstring scopename = obj.uniqueName();

  log(fstring("leaving ", scopename));

  removeScope(scopename);
}

void Nub::Log::setLogFilename(const fstring& filename)
{
DOTRACE("Nub::Log::setLogFilename");

  rutz::shared_ptr<std::ofstream> newfile
    (new std::ofstream(filename.c_str(), std::ios::out | std::ios::app));

  if (newfile->is_open() && newfile->good())
    logFile.swap(newfile);
}

void Nub::Log::setCopyToStdout(bool shouldcopy)
{
DOTRACE("Nub::Log::setCopyToStdout");
  copyToStdout = shouldcopy;
}

void Nub::log(const char* msg)
{
DOTRACE("Nub::log");
  if (copyToStdout)
    logImpl(std::cout, msg);

  if (logFile.get() != 0)
    logImpl(*logFile, msg);
}

void Nub::log(const fstring& msg)
{
DOTRACE("Nub::log");
  if (copyToStdout)
    logImpl(std::cout, msg);

  if (logFile.get() != 0)
    logImpl(*logFile, msg);
}

static const char vcid_log_cc[] = "$Id$ $URL$";
#endif // !LOG_CC_DEFINED
