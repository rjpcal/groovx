///////////////////////////////////////////////////////////////////////
//
// timinghdlr.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Jun 21 13:09:55 1999
// written: Sat Sep 23 15:32:23 2000
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

namespace GWT { class Widget; }
namespace Util { class ErrorHandler; }

class TrialBase;
class TrialEvent;

///////////////////////////////////////////////////////////////////////
//
// TimingHdlr class definition
//
///////////////////////////////////////////////////////////////////////

class TimingHdlr : public virtual IO::IoObject {
public:
  TimingHdlr();
  virtual ~TimingHdlr();

  virtual void serialize(STD_IO::ostream &os, IO::IOFlag flag) const;
  virtual void deserialize(STD_IO::istream &is, IO::IOFlag flag);
  virtual int charCount() const;

  virtual unsigned long serialVersionId() const;
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

  TrialEvent* getEvent(TimePoint time_point, int index) const;

  // Returns the elapsed time in milliseconds since the start of the
  // current trial
  int getElapsedMsec() const;

  //////////////////
  // manipulators //
  //////////////////

  int addEvent(TrialEvent* event, TimePoint time_point);
  int addEventByName(const char* event_type,
							TimePoint time_point, int msec_delay);

  /////////////
  // actions //
  /////////////

  virtual void thBeginTrial(GWT::Widget& widget,
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
