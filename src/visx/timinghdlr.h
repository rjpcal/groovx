///////////////////////////////////////////////////////////////////////
//
// timinghdlr.h
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Mon Jun 21 13:09:55 1999
// written: Wed Mar 19 12:46:28 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TIMINGHDLR_H_DEFINED
#define TIMINGHDLR_H_DEFINED

#include "io/io.h"

#include "util/stopwatch.h"

namespace Util
{
  template <class T> class Ref;
  template <class T> class SoftRef;
}

class Trial;
class TrialEvent;

///////////////////////////////////////////////////////////////////////
//
// TimingHdlr class definition
//
///////////////////////////////////////////////////////////////////////

class TimingHdlr : public IO::IoObject
{
protected:
  TimingHdlr();
public:
  static TimingHdlr* make();
  virtual ~TimingHdlr();

  virtual IO::VersionId serialVersionId() const;
  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

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

  virtual void thBeginTrial(Trial& trial);
  virtual void thResponseSeen();
  virtual void thAbortTrial();

  virtual void thHaltExpt();

private:
  TimingHdlr(const TimingHdlr&);
  TimingHdlr& operator=(const TimingHdlr&);

  class Impl;
  Impl* const rep;
};

static const char vcid_timinghdlr_h[] = "$Header$";
#endif // !TIMINGHDLR_H_DEFINED
