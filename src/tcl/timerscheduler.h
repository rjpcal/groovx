/** @file tcl/timerscheduler.h implementation of nub::scheduler using
    the tcl event scheduler */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Thu Oct 14 10:02:37 2004
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [https://github.com/rjpcal/groovx]
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

#ifndef GROOVX_TCL_TIMERSCHEDULER_H_UTC20050628162421_DEFINED
#define GROOVX_TCL_TIMERSCHEDULER_H_UTC20050628162421_DEFINED

#include "nub/scheduler.h"

namespace tcl
{
  class timer_scheduler;
}

class tcl::timer_scheduler : public nub::scheduler
{
public:
  timer_scheduler();
  virtual ~timer_scheduler() noexcept;

  virtual std::shared_ptr<nub::timer_token>
  schedule(int msec, std::function<void(void)>&& callback) override;
};

#endif // !GROOVX_TCL_TIMERSCHEDULER_H_UTC20050628162421_DEFINED
