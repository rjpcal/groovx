///////////////////////////////////////////////////////////////////////
//
// scheduler.cc
//
// Copyright (c) 2004-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Thu Oct 14 10:05:02 2004
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

#ifndef SCHEDULER_CC_DEFINED
#define SCHEDULER_CC_DEFINED

#include "scheduler.h"

Nub::TimerToken::TimerToken() {}
Nub::TimerToken::~TimerToken() throw() {}

Nub::Scheduler::~Scheduler() throw() {}

static const char vcid_scheduler_cc[] = "$Header$";
#endif // !SCHEDULER_CC_DEFINED