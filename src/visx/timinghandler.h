///////////////////////////////////////////////////////////////////////
//
// timinghandler.h
// Rob Peters
// created: Wed May 19 10:56:20 1999
// written: Thu May 20 13:00:57 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TIMINGHANDLER_H_DEFINED
#define TIMINGHANDLER_H_DEFINED

#ifndef TCL_H_DEFINED
#include <tcl.h>
#define TCL_H_DEFINED
#endif

#ifndef IO_H_DEFINED
#include "io.h"
#endif

class ExptDriver;

///////////////////////////////////////////////////////////////////////
//
// ExptTimer abstract class defintion
//
///////////////////////////////////////////////////////////////////////

enum TimeBase { IMMEDIATE, FROM_START, FROM_RESPONSE };

class ExptTimer {
public:
  // We initialize itsInterp to NULL-- this is OK because it must be
  // set with schedule() before will ever be dereferenced.
  ExptTimer(ExptDriver& ed) :
	 itsToken(NULL), itsExptDriver(ed) {}
  virtual ~ExptTimer() {
	 cancel();
  }
  void cancel() {
	 Tcl_DeleteTimerHandler(itsToken);
  }
  void schedule(int msec);

private:
  static void dummyTimerProc(ClientData clientData) {
	 ExptTimer* timer = static_cast<ExptTimer *>(clientData);
	 timer->invoke();
  }
  virtual void invoke() = 0;
  
  TimeBase itsTimeBase;
  int itsRequestedTime;
  int itsActualTime;
  Tcl_TimerToken itsToken;
protected:
  ExptDriver& itsExptDriver;
};

///////////////////////////////////////////////////////////////////////
//
// ExptTimer derived classes defintions
//
///////////////////////////////////////////////////////////////////////

class AbortTrialTimer : public ExptTimer {
public:
  AbortTrialTimer(ExptDriver& ed) : ExptTimer(ed) {}
private:
  virtual void invoke();
};

class StartTrialTimer : public ExptTimer {
public:
  StartTrialTimer(ExptDriver& ed) : ExptTimer(ed) {}
private:
  virtual void invoke();
};

class UndrawTrialTimer : public ExptTimer {
public:
  UndrawTrialTimer(ExptDriver& ed) : ExptTimer(ed) {}
private:
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
  int getAbortWait() const { return itsAbortWait; }
  int getAutosavePeriod() const { return itsAutosavePeriod; }
  int getInterTrialInterval() const { return itsInterTrialInterval; }
  int getStimDur() const { return itsStimDur; }
  int getTimeout() const { return itsTimeout; }

  void setAbortWait(int val) { itsAbortWait = val; }
  void setAutosavePeriod(int val) { itsAutosavePeriod = val; }
  void setInterTrialInterval(int val) { itsInterTrialInterval = val; }
  void setStimDur(int val) { itsStimDur = val; }
  void setTimeout(int val) { itsTimeout = val; }
  
  // Actions
  void scheduleImmediate();
  void scheduleFromStart();
  void scheduleFromResponse();

  void scheduleNextTrial();
  void scheduleAfterAbort();
  void cancelAll();

private:
  AbortTrialTimer itsAbortTimer;
  StartTrialTimer itsStartTimer;
  UndrawTrialTimer itsUndrawTrialTimer;

  int itsAbortWait;
  int itsAutosavePeriod;		  // # of trials between autosaves
  int itsInterTrialInterval;
  int itsStimDur;
  int itsTimeout;

  ExptDriver& itsExptDriver;
};

// class TimingHandler {
// public:
//   TimingHandler(ExptDriver& ed);

//   void scheduleImmediate();
//   void scheduleFromStart();
//   void scheduleFromResponse();

// private:
//   void scheduleEvents(const list<ExptEvent *>& eventList);
//   void scheduleOneEvent(ExptEvent* event)l

//   list<ExptEvent *> itsImmediateEvents;
//   list<ExptEvent *> itsStartEvents;
//   list<ExptEvent *> itsResponseEvents;

//   int itsResponseOffset;
  
//   ExptDriver& itsExptDriver;
// };

// class ExptEvent {
// public:
//   ExptEvent(int req_time) : itsRequestedTime(req_time) {}
//   virtual void action(ExptDriver& exptDriver) = 0;
// private:
//   const int itsRequestedTime;
//   int itsActualTime;
// };

// class AbortEvent : public ExptEvent {
// public:
//   virtual void action(ExptDriver& exptDriver);
// };

// class UndrawEvent : public ExptEvent {
// public:
//   virtual void action(ExptDriver& exptDriver);
// };

// class DrawEvent : public ExptEvent {
// public:
//   virtual void action(ExptDriver& exptDriver);
// };

// class ResponseSeenEvent : public ExptEvent {
// public:
//   virtual void action(ExptDriver& exptDriver) {
// 	 exptDriver->tellResponseSeen();
//   }
// };

// class RecordResponseEvent : public ExptEvent {
// public:
//   virtual void action(ExptDriver& exptDriver) {
// 	 exptDriver->processResponse(itsResponse);
//   }
// private:
//   int itsResponse;
// };

static const char vcid_timinghandler_h[] = "$Header$";
#endif // !TIMINGHANDLER_H_DEFINED
