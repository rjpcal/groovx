///////////////////////////////////////////////////////////////////////
//
// timinghdlr.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Jun 21 13:09:55 1999
// written: Fri Jun 25 12:49:43 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TIMINGHDLR_H_DEFINED
#define TIMINGHDLR_H_DEFINED

#ifndef IO_H_DEFINED
#include "io.h"
#endif

#ifndef VECTOR_DEFINED
#include <vector>
#define VECTOR_DEFINED
#endif

class TrialEvent;

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

  typedef int TimePoint;
  static const TimePoint IMMEDIATE=0;
  static const TimePoint FROM_START=1;
  static const TimePoint FROM_RESPONSE=2;
  static const TimePoint FROM_ABORT=3;

  // Accessors + Manipulators
  int getAutosavePeriod() const { return itsAutosavePeriod; }
  void setAutosavePeriod(int val) { itsAutosavePeriod = val; }

  int addEvent(TrialEvent* event, TimePoint time_point);
  int addEventByName(const char* event_type,
							TimePoint time_point, int msec_delay);
  TrialEvent* getEvent(TimePoint time_point, int index) const;

  virtual void thBeginTrial();
  virtual void thAbortTrial();
  virtual void thHaltExpt();
  virtual void thResponseSeen();

private:
  void scheduleAll(vector<TrialEvent*>& events);
  void cancelAll(vector<TrialEvent*>& events);
  void deleteAll(vector<TrialEvent*>& events);

  void cancelAll();

  vector<TrialEvent*> itsImmediateEvents;
  vector<TrialEvent*> itsStartEvents;
  vector<TrialEvent*> itsResponseEvents;
  vector<TrialEvent*> itsAbortEvents;

  int itsAutosavePeriod;
};

static const char vcid_timinghdlr_h[] = "$Header$";
#endif // !TIMINGHDLR_H_DEFINED
