///////////////////////////////////////////////////////////////////////
//
// timinghandler.h
// Rob Peters
// created: Wed May 19 10:56:20 1999
// written: Wed Mar 29 14:07:34 2000
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
public:
  // Creators
  TimingHandler();

  virtual ~TimingHandler();

  virtual void serialize(ostream &os, IOFlag flag) const;
  virtual void deserialize(istream &is, IOFlag flag);
  virtual int charCount() const;

  virtual void readFrom(Reader* reader);
  virtual void writeTo(Writer* writer) const;

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
