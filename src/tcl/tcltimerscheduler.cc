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

#include "tcl/tclmain.h"
#include "tcl/tclsafeinterp.h"

#include "util/sharedptr.h"

#include <tcl.h>

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER

namespace Tcl
{
  class TimerSchedulerToken;
}

class Tcl::TimerSchedulerToken : public Util::TimerToken
{
public:
  TimerSchedulerToken(int msec,
                      void (*callback)(void*),
                      void* clientdata);
  virtual ~TimerSchedulerToken() throw();

private:
  static void dummyCallback(void* token) throw();

  typedef void (Callback)(void*);

  Tcl_TimerToken itsToken;
  Callback* const itsCallback;
  void* const itsClientData;
};

Tcl::TimerSchedulerToken::TimerSchedulerToken(int msec,
                                              void (*callback)(void*),
                                              void* clientdata)
  :
  itsToken(Tcl_CreateTimerHandler(msec,
                                  dummyCallback,
                                  static_cast<void*>(this))),
  itsCallback(callback),
  itsClientData(clientdata)
{
DOTRACE("Tcl::TimerSchedulerToken::TimerSchedulerToken");
}

Tcl::TimerSchedulerToken::~TimerSchedulerToken() throw()
{
DOTRACE("Tcl::TimerSchedulerToken::~TimerSchedulerToken");
  Tcl_DeleteTimerHandler(itsToken);
}

void Tcl::TimerSchedulerToken::dummyCallback(void* token) throw()
{
  TimerSchedulerToken* tok = static_cast<TimerSchedulerToken*>(token);

  ASSERT(tok != 0);

  try
    {
      // BE CAREFUL: calling the client callback here may result in
      // the TimerScheduleToken object being destroyed. So, if we need
      // access to any of the token's member variables, we need to
      // make a local copy of them before calling the callback.
      (*tok->itsCallback)(tok->itsClientData);
    }
  catch(...)
    {
      Tcl::Main::interp().handleLiveException("Tcl::Timer callback",
                                              SRC_POS, true);
    }
}

Tcl::TimerScheduler::TimerScheduler()
{
DOTRACE("Tcl::TimerScheduler::TimerScheduler");
}

Tcl::TimerScheduler::~TimerScheduler() throw()
{
DOTRACE("Tcl::TimerScheduler::~TimerScheduler");
}

rutz::shared_ptr<Util::TimerToken>
Tcl::TimerScheduler::schedule(int msec,
                              void (*callback)(void*),
                              void* clientdata)
{
DOTRACE("Tcl::TimerScheduler::schedule");
  // If the requested delay is zero -- i.e., immediate -- then don't
  // bother creating a timer handler. Instead, generate a direct
  // invocation; this saves a trip into the event loop and back.
  if (msec == 0)
    {
      (*callback)(clientdata);

      // Return a null pointer, representing the fact that there is no
      // pending callback in this case.
      return rutz::shared_ptr<Util::TimerToken>();
    }

  return rutz::shared_ptr<Util::TimerToken>
    (new Tcl::TimerSchedulerToken(msec, callback, clientdata));
}

static const char vcid_tcltimerscheduler_cc[] = "$Header$";
#endif // !TCLTIMERSCHEDULER_CC_DEFINED
