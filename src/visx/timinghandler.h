///////////////////////////////////////////////////////////////////////
//
// timinghandler.h
// Rob Peters
// created: Wed May 19 10:56:20 1999
// written: Fri Oct 20 17:45:58 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TIMINGHANDLER_H_DEFINED
#define TIMINGHANDLER_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TIMINGHDLR_H_DEFINED)
#include "timinghdlr.h"
#endif

///////////////////////////////////////////////////////////////////////
//
// TimingHandler class defintion
//
///////////////////////////////////////////////////////////////////////

class TimingHandler : public TimingHdlr {
protected:
  // Creators
  TimingHandler();

public:
  static TimingHandler* make();

  virtual ~TimingHandler();

  virtual IO::VersionId serialVersionId() const;
  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

  // Accessors + Manipulators
  int getAbortWait() const; 
  int getInterTrialInterval() const; 
  int getStimDur() const; 
  int getTimeout() const; 

  void setAbortWait(int val); 
  void setInterTrialInterval(int val); 
  void setStimDur(int val); 
  void setTimeout(int val); 

private:
  int stimdur_start_id;
  int timeout_start_id;
  int iti_response_id;
  int abortwait_abort_id;
};

static const char vcid_timinghandler_h[] = "$Header$";
#endif // !TIMINGHANDLER_H_DEFINED
