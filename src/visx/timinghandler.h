///////////////////////////////////////////////////////////////////////
//
// timinghandler.h
// Rob Peters
// created: Wed May 19 10:56:20 1999
// written: Mon May 24 18:35:50 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TIMINGHANDLER_H_DEFINED
#define TIMINGHANDLER_H_DEFINED

#ifndef TCL_H_DEFINED
#include <tcl.h>
#define TCL_H_DEFINED
#endif

#ifndef VECTOR_DEFINED
#include <vector>
#define VECTOR_DEFINED
#endif

#ifndef IO_H_DEFINED
#include "io.h"
#endif

#ifdef TIME_H_DEFINED
#include <sys/time.h>
#define TIME_H_DEFINED
#endif

class ExptDriver;

///////////////////////////////////////////////////////////////////////
//
// ExptEvent abstract class defintion
//
///////////////////////////////////////////////////////////////////////

class ExptEvent {
public:
  ExptEvent(ExptDriver& ed, int msec);
  virtual ~ExptEvent();

  int getDelay() const { return itsRequestedDelay; }
  void setDelay(int msec) { itsRequestedDelay = msec; }

  void schedule();
  void cancel();

protected:
  virtual void invoke() = 0;
  
private:
  static void dummyInvoke(ClientData clientData);

  int itsRequestedDelay;
  Tcl_TimerToken itsToken;

  // Diagnostic stuff
  mutable timeval itsBeginTime;
  mutable int itsDelayErrors;
  mutable int itsInvokeCount;
protected:
  ExptDriver& itsExptDriver;
};

///////////////////////////////////////////////////////////////////////
//
// ExptEvent derived classes defintions
//
///////////////////////////////////////////////////////////////////////

class AbortTrialEvent : public ExptEvent {
public:
  AbortTrialEvent(ExptDriver& ed, int msec = 0) : ExptEvent(ed, msec) {}
protected:
  virtual void invoke();
};

class BeginTrialEvent : public ExptEvent {
public:
  BeginTrialEvent(ExptDriver& ed, int msec = 0) : ExptEvent(ed, msec) {}
protected:
  virtual void invoke();
};

class EndTrialEvent : public ExptEvent {
public:
  EndTrialEvent(ExptDriver& ed, int msec = 0) : ExptEvent(ed, msec) {}
protected:
  virtual void invoke();
};

class DrawEvent : public ExptEvent {
public:
  DrawEvent(ExptDriver& ed, int msec = 0) : ExptEvent(ed, msec) {}
protected:
  virtual void invoke();
};

class UndrawEvent : public ExptEvent {
public:
  UndrawEvent(ExptDriver& ed, int msec = 0) : ExptEvent(ed, msec) {}
protected:
  virtual void invoke();
};

///////////////////////////////////////////////////////////////////////
//
// TimingHandler class defintion
//
///////////////////////////////////////////////////////////////////////

class TimingHandler : public virtual IO {
public:
  // Creators
  TimingHandler(ExptDriver& ed);

  virtual ~TimingHandler();

  virtual void serialize(ostream &os, IOFlag flag) const;
  virtual void deserialize(istream &is, IOFlag flag);
  virtual int charCount() const;

  // Accessors + Manipulators
  int getAbortWait() const { return itsAbortWaitEvent.getDelay(); }
  int getAutosavePeriod() const { return itsAutosavePeriod; }
  int getInterTrialInterval() const { return itsBeginEvent.getDelay(); }
  int getStimDur() const { return itsUndrawEvent.getDelay(); }
  int getTimeout() const { return itsTimeoutEvent.getDelay(); }

  void setAbortWait(int val) { itsAbortWaitEvent.setDelay(val); }
  void setAutosavePeriod(int val) { itsAutosavePeriod = val; }
  void setInterTrialInterval(int val) { itsBeginEvent.setDelay(val); }
  void setStimDur(int val) { itsUndrawEvent.setDelay(val); }
  void setTimeout(int val) { itsTimeoutEvent.setDelay(val); }
  
  // Actions
  void thBeginTrial();
  void thAbortTrial();
  void thEndTrial();
  void thHaltExpt();
  void thResponseSeen();

protected:
  void cancelAll();

private:
  AbortTrialEvent itsTimeoutEvent;
  BeginTrialEvent itsBeginEvent;
  EndTrialEvent itsAbortWaitEvent;
  DrawEvent itsDrawEvent;
  UndrawEvent itsUndrawEvent;

  vector<ExptEvent *> itsBeginEvents;
  vector<ExptEvent *> itsEndEvents;
  vector<ExptEvent *> itsResponseEvents;
  vector<ExptEvent *> itsAbortEvents;

  int itsAutosavePeriod;		  // # of trials between autosaves

  ExptDriver& itsExptDriver;
};

static const char vcid_timinghandler_h[] = "$Header$";
#endif // !TIMINGHANDLER_H_DEFINED
