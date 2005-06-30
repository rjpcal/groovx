///////////////////////////////////////////////////////////////////////
//
// tcltimerscheduler.cc
//
// Copyright (c) 2004-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Thu Oct 14 10:03:46 2004
// commit: $Id$
// $HeadURL$
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

#ifndef GROOVX_TCL_TIMERSCHEDULER_CC_UTC20050628162421_DEFINED
#define GROOVX_TCL_TIMERSCHEDULER_CC_UTC20050628162421_DEFINED

#include "tcl/timerscheduler.h"

#include "tcl/eventloop.h"
#include "tcl/interp.h"

#include "rutz/sharedptr.h"

#include <tcl.h>

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

namespace Tcl
{
  class TimerSchedulerToken;
}

class Tcl::TimerSchedulerToken : public nub::timer_token
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
GVX_TRACE("Tcl::TimerSchedulerToken::TimerSchedulerToken");
}

Tcl::TimerSchedulerToken::~TimerSchedulerToken() throw()
{
GVX_TRACE("Tcl::TimerSchedulerToken::~TimerSchedulerToken");
  Tcl_DeleteTimerHandler(itsToken);
}

void Tcl::TimerSchedulerToken::dummyCallback(void* token) throw()
{
  TimerSchedulerToken* tok = static_cast<TimerSchedulerToken*>(token);

  GVX_ASSERT(tok != 0);

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
      Tcl::Main::interp().handleLiveException("timer callback",
                                              SRC_POS);
      Tcl::Main::interp().backgroundError();
    }
}

Tcl::TimerScheduler::TimerScheduler()
{
GVX_TRACE("Tcl::TimerScheduler::TimerScheduler");
}

Tcl::TimerScheduler::~TimerScheduler() throw()
{
GVX_TRACE("Tcl::TimerScheduler::~TimerScheduler");
}

rutz::shared_ptr<nub::timer_token>
Tcl::TimerScheduler::schedule(int msec,
                              void (*callback)(void*),
                              void* clientdata)
{
GVX_TRACE("Tcl::TimerScheduler::schedule");
  // If the requested delay is zero -- i.e., immediate -- then don't
  // bother creating a timer handler. Instead, generate a direct
  // invocation; this saves a trip into the event loop and back.
  if (msec == 0)
    {
      (*callback)(clientdata);

      // Return a null pointer, representing the fact that there is no
      // pending callback in this case.
      return rutz::shared_ptr<nub::timer_token>();
    }

  return rutz::shared_ptr<nub::timer_token>
    (new Tcl::TimerSchedulerToken(msec, callback, clientdata));
}

static const char vcid_groovx_tcl_timerscheduler_cc_utc20050628162421[] = "$Id$ $HeadURL$";
#endif // !GROOVX_TCL_TIMERSCHEDULER_CC_UTC20050628162421_DEFINED
