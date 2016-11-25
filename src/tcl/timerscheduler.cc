/** @file tcl/timerscheduler.cc implementation of nub::scheduler using
    the tcl event scheduler */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Thu Oct 14 10:03:46 2004
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

#include "tcl/timerscheduler.h"

#include "tcl/eventloop.h"
#include "tcl/interp.h"

#include <memory>
#include <tcl.h>

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

namespace tcl
{
  class timer_scheduler_token;
}

class tcl::timer_scheduler_token : public nub::timer_token
{
public:
  timer_scheduler_token(int msec, std::function<void(void)>&& callback);
  virtual ~timer_scheduler_token() noexcept;

private:
  static void c_callback(void* token) noexcept;

  Tcl_TimerToken                  m_token;
  std::function<void(void)> const m_callback;
};

tcl::timer_scheduler_token::
timer_scheduler_token(int msec, std::function<void(void)>&& callback)
  :
  m_token(Tcl_CreateTimerHandler(msec,
                                 c_callback,
                                 static_cast<void*>(this))),
  m_callback(std::move(callback))
{
GVX_TRACE("tcl::timer_scheduler_token::timer_scheduler_token");
}

tcl::timer_scheduler_token::~timer_scheduler_token() noexcept
{
GVX_TRACE("tcl::timer_scheduler_token::~timer_scheduler_token");
  Tcl_DeleteTimerHandler(m_token);
}

void tcl::timer_scheduler_token::c_callback(void* token) noexcept
{
  timer_scheduler_token* tok = static_cast<timer_scheduler_token*>(token);

  GVX_ASSERT(tok != nullptr);

  try
    {
      // BE CAREFUL: calling the client callback here may result in
      // the TimerScheduleToken object being destroyed. So, if we need
      // access to any of the token's member variables, we need to
      // make a local copy of them before calling the callback.
      tok->m_callback();
    }
  catch(...)
    {
      tcl::event_loop::interp().handle_live_exception("timer callback",
                                                SRC_POS);
      tcl::event_loop::interp().background_error();
    }
}

tcl::timer_scheduler::timer_scheduler()
{
GVX_TRACE("tcl::timer_scheduler::timer_scheduler");
}

tcl::timer_scheduler::~timer_scheduler() noexcept
{
GVX_TRACE("tcl::timer_scheduler::~timer_scheduler");
}

std::shared_ptr<nub::timer_token>
tcl::timer_scheduler::schedule(int msec, std::function<void(void)>&& callback)
{
GVX_TRACE("tcl::timer_scheduler::schedule");
  // If the requested delay is zero -- i.e., immediate -- then don't
  // bother creating a timer handler. Instead, generate a direct
  // invocation; this saves a trip into the event loop and back.
  if (msec == 0)
    {
      callback();

      // Return a null pointer, representing the fact that there is no
      // pending callback in this case.
      return std::shared_ptr<nub::timer_token>();
    }

  return std::make_shared<tcl::timer_scheduler_token>(msec, std::move(callback));
}
