///////////////////////////////////////////////////////////////////////
//
// timinghandler.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed May 19 10:56:20 1999
// written: Sat Jul 21 20:21:15 2001
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

  void setAbortWait(int msec);
  void setInterTrialInterval(int msec);
  void setStimDur(int msec);
  void setTimeout(int msec);

private:
  unsigned int stimdur_start_id;
  unsigned int timeout_start_id;
  unsigned int iti_response_id;
  unsigned int abortwait_abort_id;
};

static const char vcid_timinghandler_h[] = "$Header$";
#endif // !TIMINGHANDLER_H_DEFINED
