///////////////////////////////////////////////////////////////////////
//
// tcldebugscheduler.cc
//
// Copyright (c) 2004-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Fri Oct 15 17:56:46 2004
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

#ifndef TCLDEBUGSCHEDULER_CC_DEFINED
#define TCLDEBUGSCHEDULER_CC_DEFINED

#include "tcldebugscheduler.h"

#include "tcl/tclmain.h"
#include "tcl/tclsafeinterp.h"

#include "util/sharedptr.h"

#include <iostream>
#include <tcl.h>

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER

namespace Tcl
{
  class DebugSchedulerToken;
}

class Tcl::DebugSchedulerToken : public Nub::TimerToken
{
public:
  DebugSchedulerToken(Tcl::DebugScheduler* s, int id) :
    itsScheduler(s), itsId(id)
  {}

  virtual ~DebugSchedulerToken() throw()
  {
    itsScheduler->cancel(itsId);
  }

private:
  Tcl::DebugScheduler* itsScheduler;
  int itsId;
};

int Tcl::DebugScheduler::Event::nextId = 0;

Tcl::DebugScheduler::DebugScheduler() :
  itsQueue(),
  itsCurrentTime(0),
  itsToken(0)
{
DOTRACE("Tcl::DebugScheduler::DebugScheduler");
}

Tcl::DebugScheduler::~DebugScheduler() throw()
{
DOTRACE("Tcl::DebugScheduler::~DebugScheduler");
  Tcl_DeleteTimerHandler(itsToken);
}

void Tcl::DebugScheduler::cancel(int id)
{
  for (std::set<Event>::iterator
         itr = itsQueue.begin(),
         stop = itsQueue.end();
       itr != stop;
       ++itr)
    {
      if ((*itr).id == id)
        itsQueue.erase(itr);

      break;
    }
}

rutz::shared_ptr<Nub::TimerToken>
Tcl::DebugScheduler::schedule(int msec,
                              void (*callback)(void*),
                              void* clientdata)
{
DOTRACE("Tcl::DebugScheduler::schedule");
  // If the requested delay is zero -- i.e., immediate -- then don't
  // bother creating a timer handler. Instead, generate a direct
  // invocation; this saves a trip into the event loop and back.
  if (msec == 0)
    {
      (*callback)(clientdata);

      // Return a null pointer, representing the fact that there is no
      // pending callback in this case.
      return rutz::shared_ptr<Nub::TimerToken>();
    }

  Event ev;
  ev.time = itsCurrentTime + msec;
  ev.callback = callback;
  ev.clientdata = clientdata;

  itsQueue.insert(ev);

  Tcl_DeleteTimerHandler(itsToken);

  itsToken = Tcl_CreateTimerHandler(0, &idleCallback,
                                    static_cast<void*>(this));

  return rutz::shared_ptr<Nub::TimerToken>
    (new Tcl::DebugSchedulerToken(this, ev.id));
}

void Tcl::DebugScheduler::idleCallback(void* clientdata)
{
DOTRACE("Tcl::DebugScheduler::idleCallback");

  DebugScheduler* self = static_cast<DebugScheduler*>(clientdata);

  if (self->itsQueue.empty())
    return;

  Event ev = *(self->itsQueue.begin());

  self->itsQueue.erase(self->itsQueue.begin());

  self->itsCurrentTime = ev.time;

  std::cout << "waiting to run callback "
            << (void*)ev.callback
            << " with clientdata "
            << (void*)ev.clientdata
            << " at time " << ev.time
            << ":" << std::flush;

  std::cin.get();

  (*ev.callback)(ev.clientdata);

  Tcl_DeleteTimerHandler(self->itsToken);

  self->itsToken = Tcl_CreateTimerHandler(0, &idleCallback,
                                          static_cast<void*>(self));
}

static const char vcid_tcldebugscheduler_cc[] = "$Header$";
#endif // !TCLDEBUGSCHEDULER_CC_DEFINED
