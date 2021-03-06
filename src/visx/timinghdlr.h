/** @file visx/timinghdlr.h specify a trial's timing sequence in a
    psychophysics experiment */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Mon Jun 21 13:09:55 1999
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

#ifndef GROOVX_VISX_TIMINGHDLR_H_UTC20050626084015_DEFINED
#define GROOVX_VISX_TIMINGHDLR_H_UTC20050626084015_DEFINED

#include "io/io.h"

namespace nub
{
  template <class T> class ref;
  template <class T> class soft_ref;
}

class Trial;
class TrialEvent;

//  #######################################################
//  =======================================================

/// Collaborates to control a trial's timing sequence.
class TimingHdlr : public io::serializable
{
protected:
  TimingHdlr();
public:
  static TimingHdlr* make();
  virtual ~TimingHdlr() noexcept;

  virtual io::version_id class_version_id() const override;
  virtual void read_from(io::reader& reader) override;
  virtual void write_to(io::writer& writer) const override;

  typedef int TimePoint;
  static const TimePoint IMMEDIATE=0;
  static const TimePoint FROM_START=1;
  static const TimePoint FROM_RESPONSE=2;
  static const TimePoint FROM_ABORT=3;

  ///////////////
  // accessors //
  ///////////////

  nub::ref<TrialEvent> getEvent(TimePoint time_point,
                                size_t index) const;

  /** Returns the elapsed time in milliseconds since the start of the
      current trial */
  double getElapsedMsec() const;

  //////////////////
  // manipulators //
  //////////////////

  size_t addEvent(nub::ref<TrialEvent> event,
                  TimePoint time_point);
  size_t addEventByName(const char* event_type,
                        TimePoint time_point, unsigned int msec_delay);

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

#endif // !GROOVX_VISX_TIMINGHDLR_H_UTC20050626084015_DEFINED
