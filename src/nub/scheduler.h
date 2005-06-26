///////////////////////////////////////////////////////////////////////
//
// scheduler.h
//
// Copyright (c) 2004-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Thu Oct 14 10:02:53 2004
// commit: $Id$
// $HeadURL$
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

#ifndef GROOVX_NUB_SCHEDULER_H_UTC20050626084019_DEFINED
#define GROOVX_NUB_SCHEDULER_H_UTC20050626084019_DEFINED

namespace rutz
{
  template <class T> class shared_ptr;
}

namespace Nub
{
  class Scheduler;
  class TimerToken;
}

class Nub::TimerToken
{
private:
  TimerToken(const TimerToken&);
  TimerToken& operator=(const TimerToken&);

protected:
  TimerToken();

public:
  virtual ~TimerToken() throw();
};

class Nub::Scheduler
{
public:
  virtual ~Scheduler() throw();

  virtual rutz::shared_ptr<Nub::TimerToken>
  schedule(int msec,
           void (*callback)(void*),
           void* clientdata) = 0;
};

static const char vcid_groovx_nub_scheduler_h_utc20050626084019[] = "$Id$ $HeadURL$";
#endif // !GROOVX_NUB_SCHEDULER_H_UTC20050626084019_DEFINED
