///////////////////////////////////////////////////////////////////////
//
// tcltimerscheduler.cc
//
// Copyright (c) 2004-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Thu Oct 14 10:03:46 2004
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

#ifndef TCLTIMERSCHEDULER_CC_DEFINED
#define TCLTIMERSCHEDULER_CC_DEFINED

#include "tcltimerscheduler.h"

#include "util/pointers.h"

#include <tcl.h>

#include "util/trace.h"

namespace Tcl
{
  class TimerSchedulerToken;
}

class Tcl::TimerSchedulerToken : public Util::TimerToken
{
public:
  TimerSchedulerToken(Tcl_TimerToken tok);
  virtual ~TimerSchedulerToken() throw();

private:
  Tcl_TimerToken itsToken;
};

Tcl::TimerSchedulerToken::TimerSchedulerToken(Tcl_TimerToken tok)
  : itsToken(tok)
{
DOTRACE("Tcl::TimerSchedulerToken::TimerSchedulerToken");
}

Tcl::TimerSchedulerToken::~TimerSchedulerToken() throw()
{
DOTRACE("Tcl::TimerSchedulerToken::~TimerSchedulerToken");
  Tcl_DeleteTimerHandler(itsToken);
}

Tcl::TimerScheduler::TimerScheduler()
{
DOTRACE("Tcl::TimerScheduler::TimerScheduler");
}

Tcl::TimerScheduler::~TimerScheduler() throw()
{
DOTRACE("Tcl::TimerScheduler::~TimerScheduler");
}

shared_ptr<Util::TimerToken>
Tcl::TimerScheduler::schedule(int msec,
                              void (*callback)(void*),
                              void* clientdata)
{
DOTRACE("Tcl::TimerScheduler::schedule");
  Tcl_TimerToken tok =
    Tcl_CreateTimerHandler(msec, callback, clientdata);

  return shared_ptr<Util::TimerToken>
    (new Tcl::TimerSchedulerToken(tok));
}

static const char vcid_tcltimerscheduler_cc[] = "$Header$";
#endif // !TCLTIMERSCHEDULER_CC_DEFINED
