///////////////////////////////////////////////////////////////////////
//
// timinghdlr.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Jun 21 13:09:55 1999
// written: Wed Mar 29 14:07:34 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TIMINGHDLR_H_DEFINED
#define TIMINGHDLR_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IO_H_DEFINED)
#include "io.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(STOPWATCH_H_DEFINED)
#include "stopwatch.h"
#endif

class TrialEvent;
class Experiment;

///////////////////////////////////////////////////////////////////////
//
// TimingHdlr class definition
//
///////////////////////////////////////////////////////////////////////

class TimingHdlr : public virtual IO {
public:
  TimingHdlr();
  virtual ~TimingHdlr();

  virtual void serialize(ostream &os, IOFlag flag) const;
  virtual void deserialize(istream &is, IOFlag flag);
  virtual int charCount() const;

  virtual void readFrom(Reader* reader);
  virtual void writeTo(Writer* writer) const;

  typedef int TimePoint;
  static const TimePoint IMMEDIATE=0;
  static const TimePoint FROM_START=1;
  static const TimePoint FROM_RESPONSE=2;
  static const TimePoint FROM_ABORT=3;

  ///////////////
  // accessors //
  ///////////////

  int getAutosavePeriod() const;

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

  void setAutosavePeriod(int val);

  /////////////
  // actions //
  /////////////

  virtual void thBeginTrial(Experiment* expt);
  virtual void thAbortTrial(Experiment* expt);
  virtual void thResponseSeen(Experiment* expt);

  virtual void thHaltExpt(Experiment* expt);

private:
  TimingHdlr(const TimingHdlr&);
  TimingHdlr& operator=(const TimingHdlr&);

  class Impl;
  Impl* const itsImpl;
};

static const char vcid_timinghdlr_h[] = "$Header$";
#endif // !TIMINGHDLR_H_DEFINED
