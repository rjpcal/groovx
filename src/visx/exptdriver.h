///////////////////////////////////////////////////////////////////////
//
// exptdriver.h
// Rob Peters
// created: Tue May 11 13:33:50 1999
// written: Wed May 12 15:38:21 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef EXPTDRIVER_H_DEFINED
#define EXPTDRIVER_H_DEFINED

#include <tcl.h>

#include <vector>

class Expt;
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
  void schedule(int msec, Tcl_Interp* interp);

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
// ExptDriver class defintion
//
///////////////////////////////////////////////////////////////////////

class ExptDriver {
public:
  ExptDriver(Expt& expt);
  void setInterp(Tcl_Interp* interp) { itsInterp = interp; }

  Expt& expt() { return itsExpt; }
  Tcl_Interp* interp() { return itsInterp; }

  void abortTrial();
  void startTrial();
  int stopTime();
  int writeProc(const char* filename);
  int write_and_exitProc();

  void scheduleNextTrial();

  int feedbackProc(int response);

  int updateRegexpsProc();

  // The regexp 
  struct RegExp_ResponseVal {
    RegExp_ResponseVal(Tcl_RegExp rx, int rv) : regexp(rx), resp_val(rv) {}
    Tcl_RegExp regexp;
    int resp_val;
  };

  int ignoreResponse();
  int attendResponse();
  int handleResponse(const char* keysym); // returns TCL_RESULT_CODE
  
private:
  Expt& itsExpt;
  Tcl_Interp* itsInterp;

  AbortTrialTimer itsAbortTimer;
  StartTrialTimer itsStartTimer;
  UndrawTrialTimer itsUndrawTrialTimer;

  vector<RegExp_ResponseVal> itsRegexps;
};

static const char vcid_exptdriver_h[] = "$Header$";
#endif // !EXPTDRIVER_H_DEFINED
