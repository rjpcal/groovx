///////////////////////////////////////////////////////////////////////
//
// timinghdlr.h
//
// Copyright (c) 1999-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Mon Jun 21 13:09:55 1999
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

#ifndef TIMINGHDLR_H_DEFINED
#define TIMINGHDLR_H_DEFINED

#include "io/io.h"

namespace Util
{
  template <class T> class Ref;
  template <class T> class SoftRef;
}

class Trial;
class TrialEvent;

//  ###################################################################
//  ===================================================================

/// Collaborates to control a trial's timing sequence.
class TimingHdlr : public IO::IoObject
{
protected:
  TimingHdlr();
public:
  static TimingHdlr* make();
  virtual ~TimingHdlr() throw();

  virtual IO::VersionId serialVersionId() const;
  virtual void readFrom(IO::Reader& reader);
  virtual void writeTo(IO::Writer& writer) const;

  typedef int TimePoint;
  static const TimePoint IMMEDIATE=0;
  static const TimePoint FROM_START=1;
  static const TimePoint FROM_RESPONSE=2;
  static const TimePoint FROM_ABORT=3;

  ///////////////
  // accessors //
  ///////////////

  Util::Ref<TrialEvent> getEvent(TimePoint time_point,
                                 unsigned int index) const;

  /** Returns the elapsed time in milliseconds since the start of the
      current trial */
  double getElapsedMsec() const;

  //////////////////
  // manipulators //
  //////////////////

  unsigned int addEvent(Util::Ref<TrialEvent> event, TimePoint time_point);
  unsigned int addEventByName(const char* event_type,
                              TimePoint time_point, int msec_delay);

  /////////////
  // actions //
  /////////////

  void thBeginTrial(Trial& trial);
  void thResponseSeen();
  void thAbortTrial();
  void thEndTrial();

  void thHaltExpt();

private:
  TimingHdlr(const TimingHdlr&);
  TimingHdlr& operator=(const TimingHdlr&);

  class Impl;
  Impl* const rep;
};

static const char vcid_timinghdlr_h[] = "$Header$";
#endif // !TIMINGHDLR_H_DEFINED
