///////////////////////////////////////////////////////////////////////
//
// timinghdlr.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jun 21 13:09:55 1999
// written: Sat Jul 21 20:23:34 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TIMINGHDLR_H_DEFINED
#define TIMINGHDLR_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IO_H_DEFINED)
#include "io/io.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(STOPWATCH_H_DEFINED)
#include "util/stopwatch.h"
#endif

namespace GWT
{
  class Widget;
}

namespace Util
{
  class ErrorHandler;
  template <class T> class Ref;
  template <class T> class WeakRef;
}

class TrialBase;
class TrialEvent;

///////////////////////////////////////////////////////////////////////
//
// TimingHdlr class definition
//
///////////////////////////////////////////////////////////////////////

class TimingHdlr : public virtual IO::IoObject {
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
  int getElapsedMsec() const;

  //////////////////
  // manipulators //
  //////////////////

  unsigned int addEvent(Util::Ref<TrialEvent> event, TimePoint time_point);
  unsigned int addEventByName(const char* event_type,
                              TimePoint time_point, int msec_delay);

  /////////////
  // actions //
  /////////////

  virtual void thBeginTrial(Util::WeakRef<GWT::Widget> widget,
                            Util::ErrorHandler& eh, TrialBase& trial);
  virtual void thResponseSeen();
  virtual void thAbortTrial();

  virtual void thHaltExpt();

private:
  TimingHdlr(const TimingHdlr&);
  TimingHdlr& operator=(const TimingHdlr&);

  class Impl;
  Impl* const itsImpl;
};

static const char vcid_timinghdlr_h[] = "$Header$";
#endif // !TIMINGHDLR_H_DEFINED
