///////////////////////////////////////////////////////////////////////
//
// tcldebugscheduler.h
//
// Copyright (c) 2004-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Fri Oct 15 17:55:41 2004
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

#ifndef TCLDEBUGSCHEDULER_H_DEFINED
#define TCLDEBUGSCHEDULER_H_DEFINED

#include "nub/scheduler.h"

#include <set>
#include <tcl.h>

namespace Tcl
{
  class DebugScheduler;
}

class Tcl::DebugScheduler : public Nub::Scheduler
{
public:
  DebugScheduler();
  virtual ~DebugScheduler() throw();

  void cancel(int id);

  virtual rutz::shared_ptr<Nub::TimerToken>
  schedule(int msec,
           void (*callback)(void*),
           void* clientdata);

private:
  static void idleCallback(void* clientdata);

  struct Event
  {
    Event() : id(nextId++) {}

    static int nextId;

    int id;
    int time;
    void (*callback)(void*);
    void* clientdata;

    bool operator<(const Event& x) const { return time > x.time; }
  };

  std::set<Event> itsQueue;
  int itsCurrentTime;
  Tcl_TimerToken itsToken;
};

static const char vcid_tcldebugscheduler_h[] = "$Header$";
#endif // !TCLDEBUGSCHEDULER_H_DEFINED
