///////////////////////////////////////////////////////////////////////
//
// expttcl.cc
// Rob Peters
// created: Mon Mar  8 03:18:40 1999
// written: Mon May 10 18:47:42 1999
// $Id$
//
// This file defines the procedures that provide the Tcl interface to
// the Expt class, and defines several Tcl variables that are provided
// within the Tcl Expt:: namespace.
//
///////////////////////////////////////////////////////////////////////

#ifndef EXPTTCL_CC_DEFINED
#define EXPTTCL_CC_DEFINED

#include "expttcl.h"

#include <iostream.h>
#include <fstream.h>
#include <tcl.h>
#include <string>
#include <vector>
#include <cstring>

#include "soundtcl.h"
#include "objlisttcl.h"
#include "tlisttcl.h"
#include "expt.h"
#include "errmsg.h"
#include "tcllink.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

///////////////////////////////////////////////////////////////////////
//
// Expt Tcl package declarations
//
///////////////////////////////////////////////////////////////////////

namespace ExptTcl { Expt& getExpt();

  // Tcl command procedures
  Tcl_ObjCmdProc beginCmd;
  Tcl_ObjCmdProc currentStimclassCmd;
  Tcl_ObjCmdProc currentTrialCmd;
  Tcl_ObjCmdProc handleResponseCmd;
  Tcl_ObjCmdProc initCmd;
  Tcl_ObjCmdProc intVarCmd;     // used to get/set an int value
  Tcl_ObjCmdProc isCompleteCmd;
  Tcl_ObjCmdProc numTrialsCmd;
  Tcl_ObjCmdProc numCompletedCmd;
  Tcl_ObjCmdProc pauseCmd;
  Tcl_ObjCmdProc prevResponseCmd;
  Tcl_ObjCmdProc readCmd;
  Tcl_ObjCmdProc stopCmd;
  Tcl_ObjCmdProc stringVarCmd;  // used to get/set a string value
  Tcl_ObjCmdProc trialDescriptionCmd;
  Tcl_ObjCmdProc useFeedbackCmd;
  Tcl_ObjCmdProc verboseCmd;
  Tcl_ObjCmdProc writeCmd;
  Tcl_ObjCmdProc write_and_exitCmd;

  // Tcl command procedures used only for testing the Expt class
#ifdef LOCAL_TEST
  Tcl_ObjCmdProc abortTrialCmd;
  Tcl_ObjCmdProc beginTrialCmd;
  Tcl_ObjCmdProc recordResponseCmd;
  Tcl_ObjCmdProc undrawTrialCmd;
#endif

  // This variable is linked into Tcl, so we can tell from Tcl if
  // testing functions have been enabled.
#ifdef LOCAL_TEST
  int haveTest=1;
#else 
  int haveTest=0;
#endif

  // Helper procedures

  // Graphics/responsePolicy helpers
  int bindToglProc(Tcl_Interp* interp);
  int feedbackProc(Tcl_Interp* interp);
  int unbindToglProc(Tcl_Interp* interp);
  int updateRegexpsProc(Tcl_Interp* interp);

  // OS/Tcl provided helpers
  string getDateStringProc(Tcl_Interp* interp);
  string getSubjectKeyProc(Tcl_Interp* interp);
};

using namespace ExptTcl;

///////////////////////////////////////////////////////////////////////
//
// ExptDriver class
//
///////////////////////////////////////////////////////////////////////

class ExptDriver {
public:
  ExptDriver(Expt& expt) : itsExpt(expt), itsInterp(NULL) {}
  void setInterp(Tcl_Interp* interp) { itsInterp = interp; }

  Expt& expt() { return itsExpt; }
  Tcl_Interp* interp() { return itsInterp; }

  void abortTrial();
  void startTrial();
  int stopExpt();
  int writeProc(const char* filename);
  int write_and_exitProc();
  
private:
  Expt& itsExpt;
  Tcl_Interp* itsInterp;
};

ExptDriver& getExptDriver(Tcl_Interp* interp = NULL) {
  static ExptDriver theExptDriver(getExpt());
  if (interp) {
	 theExptDriver.setInterp(interp);
  }
  return theExptDriver;
}

///////////////////////////////////////////////////////////////////////
//
// Timer callbacks
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
  void schedule(int msec, Tcl_Interp* interp) {
	 cancel();
	 itsExptDriver.setInterp(interp);
	 itsToken = Tcl_CreateTimerHandler(msec, dummyTimerProc, 
												  static_cast<ClientData>(this));
  }
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

class AbortTrialTimer : public ExptTimer {
public:
  AbortTrialTimer(ExptDriver& ed) : ExptTimer(ed) {}
private:
  virtual void invoke() {
	 cerr << "AbortTrialTimer::invoke" << endl;
	 itsExptDriver.abortTrial();
  }
};

class StartTrialTimer : public ExptTimer {
public:
  StartTrialTimer(ExptDriver& ed) : ExptTimer(ed) {}
private:
  virtual void invoke() {
	 cerr << "StartTrialTimer::invoke" << endl;
	 itsExptDriver.startTrial();
  }
};

class UndrawTrialTimer : public ExptTimer {
public:
  UndrawTrialTimer(ExptDriver& ed) : ExptTimer(ed) {}
private:
  virtual void invoke() {
	 cerr << "UndrawTrialTimer::invoke" << endl;
	 itsExptDriver.expt().undrawTrial();
  }
};

AbortTrialTimer abortTimer(getExptDriver());
StartTrialTimer startTimer(getExptDriver());
UndrawTrialTimer undrawTrialTimer(getExptDriver());


///////////////////////////////////////////////////////////////////////
//
// Local stuff that needs to be better organized
//
///////////////////////////////////////////////////////////////////////

namespace {

  // This helper class can be used to ensure that a Tcl_Obj* is not
  // garbage-collected within a particular scope, by incrementing the
  // ref count on construction of the lock, and decrementing the ref
  // count on destruction of the lock. This allows Tcl_Obj*'s to
  // behave more according to normal scoping rules.
  class TclObjLock {
  public:
    TclObjLock(Tcl_Obj* obj) : itsObj(obj) { Tcl_IncrRefCount(itsObj); }
    ~TclObjLock() { Tcl_DecrRefCount(itsObj); }
  private:
    Tcl_Obj* itsObj;
  };

  // The regexp 
  struct RegExp_ResponseVal {
    RegExp_ResponseVal(Tcl_RegExp rx, int rv) : regexp(rx), resp_val(rv) {}
    Tcl_RegExp regexp;
    int resp_val;
  };
  vector<RegExp_ResponseVal> theRegexps;

  // The struct String_Get_Set, and an array String_Get_Set[] in
  // stringVarCmd, are used to access Expt get/set functions that
  // handle strings
  typedef const string& (Expt::* PExpt_string_getter) () const;
  typedef void (Expt::* PExpt_string_setter) (const string&);

  struct String_Get_Set {
    const char* name;
    PExpt_string_getter getter;
    PExpt_string_setter setter;
  };

  // The struct Int_Get_Set, and an array Int_Get_Set[] in intVarCmd,
  // are used to access Expt get/set functions that handle ints
  typedef int (Expt::* PExpt_int_getter) () const;
  typedef void (Expt::* PExpt_int_setter) (int);

  struct Int_Get_Set {
    const char* name;
    PExpt_int_getter getter;
    PExpt_int_setter setter;
  };
};

///////////////////////////////////////////////////////////////////////
//
// Expt Tcl definitions
//
///////////////////////////////////////////////////////////////////////

inline Expt& ExptTcl::getExpt() {
DOTRACE("ExptTcl::getExpt");
  static Expt theExpt(TlistTcl::getTlist(), 0);
  return theExpt;
}

//--------------------------------------------------------------------
//
// ExptTcl::abortTrial --
//
// Helper procedure that is invoked as a callback for the abort timer.
//
// Results:
//
//
//
// Side effects:
//
//
//
//--------------------------------------------------------------------

void ExptDriver::abortTrial() {
DOTRACE("ExptDriver::abortTrial");
  int result = TCL_OK;

  result = unbindToglProc(itsInterp);
  Assert(result == TCL_OK);

  result = SoundTcl::playProc(itsInterp, NULL, "err");
  Assert(result == TCL_OK);

  itsExpt.abortTrial();

  startTimer.schedule(itsExpt.getAbortWait(), itsInterp);
}

//--------------------------------------------------------------------
//
// ExptTcl::abortTrialCmd --
//
// Abort the current trial of the current experiment.
//
// Results:
// none.
//
// Side effects: 
// The current (to be aborted) trial is put at the end of the trial
// sequence in the current Expt, without recording any response for
// that trial. The next call to beginTrial will start the same trial
// that would have started if the current trial had been completed
// normally, instead of begin aborted.
//
//--------------------------------------------------------------------

#ifdef LOCAL_TEST
int ExptTcl::abortTrialCmd(ClientData, Tcl_Interp* interp,
                           int objc, Tcl_Obj* const objv[]) {
DOTRACE("ExptTcl::abortTrialCmd");
  if (objc != 1) {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }

  getExpt().abortTrial();
  return TCL_OK;
}
#endif

//--------------------------------------------------------------------
//
// ExptTcl::beginCmd --
//
// Creates the necessary screen bindings for start, pause, and quit,
// then begins the current trial (probably the first trial) of the
// current Expt. Record the time when the experiment began.
//
// Results:
// none
//
// Side effects:
// startTrial is called, which displays a trial, and generates the 
// timer callbacks associated with a trial.
//
//--------------------------------------------------------------------

int ExptTcl::beginCmd(ClientData, Tcl_Interp *interp,
                      int objc, Tcl_Obj *const objv[]) {
DOTRACE("ExptTcl::beginCmd");
  if (objc != 1) {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }

  int result = TCL_OK;

  // Create the quit key binding
  static Tcl_Obj* bindQuitCmdObj = 
    Tcl_NewStringObj("bind .togl <KeyPress-q> { Expt::write_and_exit }", -1);
  static TclObjLock quit_lock_(bindQuitCmdObj);

  result = Tcl_EvalObjEx(interp, bindQuitCmdObj, TCL_EVAL_GLOBAL);
  if (result != TCL_OK) return result;

  // Create the pause key binding
  static Tcl_Obj* bindPauseCmdObj = 
    Tcl_NewStringObj("bind .togl <KeyPress-p> { Expt::pause }", -1);
  static TclObjLock pause_lock_(bindPauseCmdObj);

  result = Tcl_EvalObjEx(interp, bindPauseCmdObj, TCL_EVAL_GLOBAL);
  if (result != TCL_OK) return result;

  // Destroy the experiment start key binding
  static Tcl_Obj* unbindStartCmdObj =
    Tcl_NewStringObj("bind .togl <KeyPress-s> {}", -1);
  static TclObjLock start_lock_(unbindStartCmdObj);

  result = Tcl_EvalObjEx(interp, unbindStartCmdObj, TCL_EVAL_GLOBAL);
  if (result != TCL_OK) return result;

  // Force the focus to the Togl widget
  static Tcl_Obj* focusForceCmdObj = 
	 Tcl_NewStringObj("focus -force .togl", -1);
  static TclObjLock focus_lock_(focusForceCmdObj);

  result = Tcl_EvalObjEx(interp, focusForceCmdObj, TCL_EVAL_GLOBAL);
  if (result != TCL_OK) return result;

  getExptDriver(interp).startTrial();

  return result;
}

//--------------------------------------------------------------------
//
// ExptTcl::beginTrialCmd --
//
// Start the next trial in the current experiment.
//
// Results: 
// If Expt::verbose is true, a description of the current trial is
// returned, otherwise nothing.
//
// Side effects:
// Begin the next trial in the current Expt. The trial is drawn, and
// the timer to measure response time is started. The current trial is 
// not changed until a call either to recordResponse or to abortTrial;
// thus, multiple calls in a row to beginTrial will simply show the 
// same trial repeatedly, although the response time timer will be
// restarted each time.
//
//--------------------------------------------------------------------

#ifdef LOCAL_TEST
int ExptTcl::beginTrialCmd(ClientData, Tcl_Interp* interp,
                           int objc, Tcl_Obj* const objv[]) {
DOTRACE("ExptTcl::beginTrialCmd");
  if (objc > 2) {
    Tcl_WrongNumArgs(interp, 1, objv, "?verbose?");
    return TCL_ERROR;
  }

  int verbose = 0;
  if (objc >= 2) {
    if (Tcl_GetIntFromObj(interp, objv[1], &verbose) != TCL_OK) return TCL_ERROR;
  }

  Expt& expt = getExpt();
  expt.beginTrial();

  if (verbose) {
    Tcl_SetObjResult(interp, Tcl_NewStringObj(expt.trialDescription(), -1));
  }
  return TCL_OK;
}
#endif

//--------------------------------------------------------------------
//
// ExptTcl::bindToglProc --
//
//
//
// Results:
//
//
//
// Side effects:
//
//
//
//--------------------------------------------------------------------

int ExptTcl::bindToglProc(Tcl_Interp* interp) {
DOTRACE("ExptTcl::bindToglProc");
  static Tcl_Obj* bindCmdObj = 
    Tcl_NewStringObj("bind .togl <KeyPress> { Expt::handleResponse %K }", -1);
  static TclObjLock lock_(bindCmdObj);
  return Tcl_EvalObjEx(interp, bindCmdObj, TCL_EVAL_GLOBAL);
}

//--------------------------------------------------------------------
//
// ExptTcl::currentStimClassCmd --
//
// Results:
// Returns the stimclass of the current trial in the current experiment.
//
// Side effects:
// none.
//
//--------------------------------------------------------------------

int ExptTcl::currentStimclassCmd(ClientData, Tcl_Interp* interp,
                                 int objc, Tcl_Obj* const objv[]) {
DOTRACE("ExptTcl::currentStimclassCmd");
  if (objc != 1) {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }

  Tcl_SetObjResult(interp, Tcl_NewIntObj(getExpt().currentStimClass()));
  return TCL_OK;
}

//--------------------------------------------------------------------
//
// ExptTcl::currentTrialCmd --
//
// Results:
// Returns the index of the current trial in the current experiment.
//
// Side effects:
// none.
//
//--------------------------------------------------------------------

int ExptTcl::currentTrialCmd(ClientData, Tcl_Interp* interp,
                             int objc, Tcl_Obj* const objv[]) {
DOTRACE("ExptTcl::currentTrialCmd");
  if (objc != 1) {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }
  Tcl_SetObjResult(interp, Tcl_NewIntObj(getExpt().currentTrial()));
  return TCL_OK;
}

//--------------------------------------------------------------------
//
// ExptTcl::feedbackProc --
//
// Call the user-defined feedback procedure with a given response
// value.
//
// Results:
// none.
//
// Side effects:
// Whatever the user-defined procedure "feedback" does.
//
//--------------------------------------------------------------------

int ExptTcl::feedbackProc(Tcl_Interp* interp, int response) {
DOTRACE("ExptTcl::feedbackProc");
  static Tcl_Obj* feedbackObj = 
    Tcl_NewStringObj("::feedback", -1);
  static TclObjLock lock_(feedbackObj);
  static Tcl_Obj* feedbackObjv[2] = { feedbackObj, NULL };
  feedbackObjv[1] = Tcl_NewIntObj(response);
  TclObjLock lock2_(feedbackObjv[1]);
  return Tcl_EvalObjv(interp, 2, feedbackObjv, TCL_EVAL_GLOBAL);
}

//--------------------------------------------------------------------
//
// ExptTcl::getDateStringProc --
//
//
//
// Results:
//
//
//
// Side effects:
//
//
//
//--------------------------------------------------------------------

string ExptTcl::getDateStringProc(Tcl_Interp* interp) {
DOTRACE("ExptTcl::getDateStringProc");
  static Tcl_Obj* dateStringCmdObj =
    Tcl_NewStringObj("clock format [clock seconds]", -1);
  static TclObjLock lock_(dateStringCmdObj);

  int result = Tcl_EvalObjEx(interp, dateStringCmdObj, TCL_EVAL_GLOBAL);
  if (result != TCL_OK) return string();

  return string(Tcl_GetStringResult(interp));
}

//--------------------------------------------------------------------
//
// ExptTcl::getSubjectKeyProc --
//
// Determine the subject key (probably the subject's initials) from
// the current directory.
//
// Results: 
// Returns the subject key as a string, or the empty string if an error
// occurred.
//
// Side effects:
// none.
//
//--------------------------------------------------------------------

string ExptTcl::getSubjectKeyProc(Tcl_Interp* interp) {
DOTRACE("ExptTcl::getSubjectKeyProc");
  static Tcl_Obj* subjectKeyCmdObj = 
    Tcl_NewStringObj("file tail [pwd]", -1);
  static TclObjLock lock_(subjectKeyCmdObj);

  int result = Tcl_EvalObjEx(interp, subjectKeyCmdObj, TCL_EVAL_GLOBAL);
  if (result != TCL_OK) return string();

  // Get the result, and remove an optional leading 'human_', if
  // present
  const char* key = Tcl_GetStringResult(interp);
  if ( strncmp(key, "human_", 6) == 0 ) {
    key += 6;
  }

  return string(key);
}

//--------------------------------------------------------------------
//
// ExptTcl::handleResponseCmd --
//
// Results:
//
//
//
// Side effects:
//
//
//
//--------------------------------------------------------------------

int ExptTcl::handleResponseCmd(ClientData, Tcl_Interp *interp,
                               int objc, Tcl_Obj *const objv[]) {
DOTRACE("ExptTcl::handleResponseCmd");
  if (objc != 2) {
    Tcl_WrongNumArgs(interp, 1, objv, "keysym");
    return TCL_ERROR;
  }

  int result = TCL_OK;

  result = unbindToglProc(interp);
  if (result != TCL_OK) return result;

  abortTimer.cancel();
  undrawTrialTimer.cancel();

  Expt& expt = getExpt();

  expt.undrawTrial();

  const char* keysym = Tcl_GetString(objv[1]);

  // Get exactly one character -- the last character -- in keysym
  // (just before the terminating '\0'). This is the character that we
  // will take as the response. (The last character must be extracted
  // to handle the numeric keypad, where the keysysms are 'KP_0',
  // 'KP_3', etc., and we want just the 0 or the 3).
  const char* last_char = keysym;
  while ( *(last_char+1) != '\0' ) { last_char++; }
    
  int resp = -1;
  for (int i = 0; i < theRegexps.size(); i++) {
    int found = Tcl_RegExpExec(interp, theRegexps[i].regexp, 
                               last_char, last_char);
    if (found == -1) return TCL_ERROR;
    if (found == 1) {
      resp = theRegexps[i].resp_val;
      break;
    }
  }
    
  // If resp is -1, then the keypress did not match any of the regexps
  // in theRegexps, so the response was invalid, and we abort the
  // trial.
  if (resp == -1) {
    getExptDriver(interp).abortTrial();
    return TCL_OK;
  }

  // If useFeedback is true, we call the user-defined feedback
  // function with resp as its argument.
  if (expt.getUseFeedback()) {
    result = feedbackProc(interp, resp);
    if (result != TCL_OK) return result;
  }

  expt.recordResponse(resp);

  if (expt.getVerbose()) {
    cout << "response " << resp << endl;
  }

  if (expt.isComplete()) {
    cout << "expt complete" << endl;
    return getExptDriver(interp).write_and_exitProc();
  }    
  
  startTimer.schedule(expt.getInterTrialInterval(), interp);

  return result;
} // handleResponseCmd

//--------------------------------------------------------------------
//
// ExptTcl::initCmd --
//
// Initialize the current experiment by generating a new trial
// sequence, and recording certain environment information for
// posterity, such as the current time, the host computer, and the
// current directory.
//
// Results:
// none.
//
// Side effects:
// A new random trial sequence is generated for the current Expt, based
// on the value of 'rand_seed', in which each Trial is listed 'repeat' 
// number of times.
//
//--------------------------------------------------------------------

int ExptTcl::initCmd(ClientData, Tcl_Interp* interp,
                     int objc, Tcl_Obj* const objv[]) {
DOTRACE("ExptTcl::initCmd");
  if (objc != 3) {
    Tcl_WrongNumArgs(interp, 1, objv, "repeat rand_seed");
    return TCL_ERROR;
  }

  int repeat;
  if (Tcl_GetIntFromObj(interp, objv[1], &repeat) != TCL_OK) return TCL_ERROR;

  int rand_seed;
  if (Tcl_GetIntFromObj(interp, objv[2], &rand_seed) != TCL_OK) return TCL_ERROR;

  // Get a nice human-readable format of the date+time
  string date = getDateStringProc(interp);
  if (!date.size()) return TCL_ERROR;

  string host = Tcl_GetVar2(interp, "env", "HOST", 
                            TCL_GLOBAL_ONLY|TCL_LEAVE_ERR_MSG);

  // Get the key for the experiment subject (probably his or her
  // initials)
  string subject = getSubjectKeyProc(interp);
  if (!date.size()) return TCL_ERROR;

  getExpt().init(repeat, rand_seed, date, host, subject);
  return TCL_OK;
}

//--------------------------------------------------------------------
//
// ExptTcl::intVarCmd --
//
//
//
// Results:
//
//
//
// Side effects:
//
//
//
//--------------------------------------------------------------------

int ExptTcl::intVarCmd(ClientData, Tcl_Interp *interp,
                       int objc, Tcl_Obj *const objv[]) {
DOTRACE("ExptTcl::intVarCmd");
  if (objc != 1 && objc != 2) {
    Tcl_WrongNumArgs(interp, 1, objv, "?new_value?");
    return TCL_ERROR;
  }

  // Get the param that we will either retrieve or set, and remove a
  // possible leading namespace qualifier
  const char* param = Tcl_GetString(objv[0]);
  Tcl_Obj* param_obj = NULL;
  if ( strncmp(param, "Expt::", 6) == 0 ) {
    param +=6;
    param_obj = Tcl_NewStringObj(param, -1);
  }
  else {
    param_obj = objv[0];
  }

  Int_Get_Set isg_params[] = {
    { "abortWait", &Expt::getAbortWait, &Expt::setAbortWait },
    { "autosavePeriod", &Expt::getAutosavePeriod, &Expt::setAutosavePeriod },
    { "interTrialInterval", &Expt::getInterTrialInterval, 
      &Expt::setInterTrialInterval },
    { "stimDur", &Expt::getStimDur, &Expt::setStimDur },
    { "timeout", &Expt::getTimeout, &Expt::setTimeout },
    { NULL, NULL, NULL }
  };

  // Find the index of the param in isg_params[]
  int offset = sizeof(Int_Get_Set);
  int index;
  if (Tcl_GetIndexFromObjStruct(interp, param_obj,
                                reinterpret_cast<char **>(isg_params), offset,
                                "property", 0, &index) != TCL_OK)
    return TCL_ERROR;

  // Retrieve current value
  if (objc == 1) {
    int val = (getExpt().*isg_params[index].getter) ();
    Tcl_SetObjResult(interp, Tcl_NewIntObj(val));
  }

  // Set new value
  else if (objc == 2) {
    int new_val;
    if (Tcl_GetIntFromObj(interp, objv[1], &new_val) != TCL_OK)
      return TCL_ERROR;
    (getExpt().*isg_params[index].setter) (new_val);
  }

  return TCL_OK;
} // intVarCmd

//--------------------------------------------------------------------
//
// ExptTcl::isCompleteCmd --
//
// Results:
// Returns true if the current experiment is complete (i.e. all trials
// in the trial sequence have finished successfully), false otherwise.
//
// Side effects:
// none
//
//--------------------------------------------------------------------

int ExptTcl::isCompleteCmd(ClientData, Tcl_Interp* interp,
                           int objc, Tcl_Obj* const objv[]) {
DOTRACE("ExptTcl::isCompleteCmd");
  if (objc != 1) {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }
  Tcl_SetObjResult(interp, Tcl_NewBooleanObj(getExpt().isComplete()));
  return TCL_OK;
}

//--------------------------------------------------------------------
//
// ExptTcl::numCompletedCmd --
//
// Results:
// Returns the number of trials that have been successfully completed
// in the current Expt. This number will not include trials that have
// been aborted either due to an invalid response or due to a timeout.
//
// Side effects:
// none.
//
//
//--------------------------------------------------------------------

int ExptTcl::numCompletedCmd(ClientData, Tcl_Interp* interp,
                             int objc, Tcl_Obj* const objv[]) {
DOTRACE("ExptTcl::numCompletedCmd");
  if (objc != 1) {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }
  Tcl_SetObjResult(interp, Tcl_NewIntObj(getExpt().numCompleted()));
  return TCL_OK;
}

//--------------------------------------------------------------------
//
// ExptTcl::numTrialsCmd --
//
// Results:
// Returns the total number of trials that comprise the current Expt.
//
// Side effects:
// none.
//
//--------------------------------------------------------------------

int ExptTcl::numTrialsCmd(ClientData, Tcl_Interp* interp,
                          int objc, Tcl_Obj* const objv[]) {
DOTRACE("ExptTcl::numTrialsCmd");
  if (objc != 1) {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }
  Tcl_SetObjResult(interp, Tcl_NewIntObj(getExpt().numTrials()));
  return TCL_OK;
}

//--------------------------------------------------------------------
//
// ExptTcl::pauseCmd --
//
// Results:
//
//
//
// Side effects:
//
//
//
//--------------------------------------------------------------------

int ExptTcl::pauseCmd(ClientData, Tcl_Interp *interp,
                      int objc, Tcl_Obj *const objv[]) {
DOTRACE("ExptTcl::pauseCmd");
  if (objc != 1) {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }

  int result = TCL_OK;

  Expt& expt = getExpt();

  getExptDriver(interp).stopExpt();

  static Tcl_Obj* pauseMsgCmdObj = 
    Tcl_NewStringObj("tk_messageBox -default ok -icon info "
                     "-title \"Pause\" -type ok "
                     "-message \"Experiment paused. Click OK to continue.\"",
                     -1);
  static TclObjLock lock_(pauseMsgCmdObj);

  result = Tcl_EvalObjEx(interp, pauseMsgCmdObj, TCL_EVAL_GLOBAL);
  if (result != TCL_OK) return result;

  startTimer.schedule(expt.getInterTrialInterval(), interp);

  return result;
}

//--------------------------------------------------------------------
//
// ExptTcl::prevResponseCmd --
//
// Results:
// Returns the last valid (but not necessarily "correct") response that
// was recorded in the current Expt.
//
// Side effects:
// none.
//
//--------------------------------------------------------------------

int ExptTcl::prevResponseCmd(ClientData, Tcl_Interp* interp,
                             int objc, Tcl_Obj* const objv[]) {
DOTRACE("ExptTcl::prevResponseCmd");
  if (objc != 1) {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }
  Tcl_SetObjResult(interp, Tcl_NewIntObj(getExpt().prevResponse()));
  return TCL_OK;
}

//--------------------------------------------------------------------
//
// ExptTcl::readCmd --
//
// Restore the current Expt to a previously saved state by reading a
// file.
//
// Results:
// Returns an error if the filename provided is invalid, or if the
// file is corrupt or contains invalid data.
//
// Side effects:
// The current Expt and all of its components (Tlist, ObjList, PosList)
// are restored to the state described in the file.
//
//--------------------------------------------------------------------

int ExptTcl::readCmd(ClientData, Tcl_Interp* interp,
                     int objc, Tcl_Obj* const objv[]) {
DOTRACE("ExptTcl::readCmd");
  if (objc != 2) {
    Tcl_WrongNumArgs(interp, 1, objv, "filename");
    return TCL_ERROR;
  }
  
  char* filename = Tcl_GetString(objv[1]);
  if (filename == NULL) return TCL_ERROR;

  try {
    ifstream ifs(filename);
    if (ifs.fail()) throw IoFilenameError(filename);
    getExpt().deserialize(ifs, IO::BASES|IO::TYPENAME);
  }
  catch (IoError& err) {
    err_message(interp, objv, err.info().c_str());
    return TCL_ERROR;
  }
  return TCL_OK;
}

//--------------------------------------------------------------------
//
// ExptTcl::recordResponseCmd --
//
// Record a response to the current trial in the current Expt, and 
// prepare the Expt for the next trial.
//
// Results:
// none.
//
// Side effects:
// The response is recorded for the current trial, and the Expt's
// trial sequence index is incremented. In this way, the next call to 
// beginTrial will start the next trial. Also, the next call to 
// prevResponse will return the response that was recorded in the
// present command.
//
//--------------------------------------------------------------------

#ifdef LOCAL_TEST
int ExptTcl::recordResponseCmd(ClientData, Tcl_Interp* interp,
                               int objc, Tcl_Obj* const objv[]) {
DOTRACE("ExptTcl::recordResponseCmd");
  if (objc != 2) {
    Tcl_WrongNumArgs(interp, 1, objv, "response");
    return TCL_ERROR;
  }

  int response;
  if (Tcl_GetIntFromObj(interp, objv[1], &response) != TCL_OK)
    return TCL_ERROR;

  getExpt().recordResponse(response);
  return TCL_OK;
}
#endif

//--------------------------------------------------------------------
//
// ExptTcl::startTrial --
//
// Helper procedure that is invoked as a callback for the start
// timer. When this procedure is invoked, a new trial is begun, and
// the appropriate undraw and abort timer callbacks are issued.
//
// Results:
// None.
//
// Side effects:
// May create or overwrite an autosave file. Creates two timer 
// callbacks-- one to undraw the trial, another to abort the trial.
//
//--------------------------------------------------------------------

void ExptDriver::startTrial() {
DOTRACE("ExptDriver::startTrial");
  int result = TCL_OK;
  
  undrawTrialTimer.cancel();
  abortTimer.cancel();

  if ( itsExpt.needAutosave() ) {
	 result = writeProc(itsExpt.getAutosaveFile().c_str());
    Assert(result == TCL_OK);
  }

  // Clear the event queue before we rebind to KeyPress events
  while (Tcl_DoOneEvent(TCL_ALL_EVENTS|TCL_DONT_WAIT) != 0) {
    ; // Empty loop body
  }

  result = bindToglProc(itsInterp);
  Assert(result == TCL_OK);

  itsExpt.beginTrial();

  undrawTrialTimer.schedule(itsExpt.getStimDur(), itsInterp);
  abortTimer.schedule(itsExpt.getTimeout(), itsInterp);

  if (itsExpt.getVerbose()) {
    cerr << itsExpt.trialDescription() << endl;
  }
}

//--------------------------------------------------------------------
//
// ExptTcl::stopCmd --
//
// Results:
//
//
//
// Side effects:
//
//
//
//--------------------------------------------------------------------

int ExptTcl::stopCmd(ClientData, Tcl_Interp *interp,
                     int objc, Tcl_Obj *const objv[]) {
DOTRACE("ExptTcl::stopCmd");
  if (objc != 1) {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }

  getExptDriver(interp).stopExpt();
  return TCL_OK;
}

//--------------------------------------------------------------------
//
// ExptTcl::stopExpt --
//
// Helper procedure that cancels outstanding timers, unbinds KeyPress
// events from the screen window, clears the screen, and aborts the
// current trial.
//
// Results:
//
//
//
// Side effects:
//
//
//
//--------------------------------------------------------------------

int ExptDriver::stopExpt() {
DOTRACE("ExptDriver::stopExpt");
  abortTimer.cancel();
  startTimer.cancel();
  undrawTrialTimer.cancel();

  int result = TCL_OK;
  result = unbindToglProc(itsInterp);
  if (result != TCL_OK) return result;

  itsExpt.undrawTrial();
  itsExpt.abortTrial();
  return TCL_OK;
}

//--------------------------------------------------------------------
//
// ExptTcl::stringVarCmd --
//
//
//
// Results:
//
//
//
// Side effects:
//
//
//
//--------------------------------------------------------------------

int ExptTcl::stringVarCmd(ClientData, Tcl_Interp *interp,
                          int objc, Tcl_Obj *const objv[]) {
DOTRACE("ExptTcl::stringVarCmd");
  if (objc != 1 && objc != 2) {
    Tcl_WrongNumArgs(interp, 1, objv, "?new_value?");
    return TCL_ERROR;
  }

  // Get the param that we will either retrieve or set, and remove a
  // possible leading namespace qualifier
  const char* param = Tcl_GetString(objv[0]);
  Tcl_Obj* param_obj = NULL;
  if ( strncmp(param, "Expt::", 6) == 0 ) {
    param +=6;
    param_obj = Tcl_NewStringObj(param, -1);
  }
  else {
    param_obj = objv[0];
  }

  String_Get_Set ssg_params[] = {
    { "endDate", &Expt::getEndDate, &Expt::setEndDate },
    { "autosaveFile", &Expt::getAutosaveFile, &Expt::setAutosaveFile },
    { "keyRespPairs", &Expt::getKeyRespPairs, &Expt::setKeyRespPairs },
    { NULL, NULL, NULL }
  };

  // Find the index of the param in ssg_params[]
  int offset = sizeof(String_Get_Set);
  int index;
  if (Tcl_GetIndexFromObjStruct(interp, param_obj,
                                reinterpret_cast<char **>(ssg_params), offset,
                                "property", 0, &index) != TCL_OK)
    return TCL_ERROR;

  // Retrieve current value
  if (objc == 1) {
    const string& str = (getExpt().*ssg_params[index].getter) ();
    Tcl_SetObjResult(interp, Tcl_NewStringObj(str.c_str(),-1));
  }

  // Set new value
  else if (objc == 2) {
    (getExpt().*ssg_params[index].setter) (Tcl_GetString(objv[1]));

    if (index == 2) {           // keyRespPairs
      if (updateRegexpsProc(interp) != TCL_OK) {
        return TCL_ERROR;
      }
    }
  }

  return TCL_OK;
} // stringVarCmd

//--------------------------------------------------------------------
//
// ExptTcl::trialDescriptionCmd --
//
// Get a human-readable description of the current trial in the 
// current Expt.
//
// Results:
// Returns a human(programmer)-readable description of the current
// trial that shows the trial's id, the trial's type, the id's of the
// objects that are displayed in the trial, the categories of those
// objects, and the number of completed trials and number of total
// trials.
//
// Side effects:
// none
//
//--------------------------------------------------------------------

int ExptTcl::trialDescriptionCmd(ClientData, Tcl_Interp* interp,
                                 int objc, Tcl_Obj* const objv[]) {
DOTRACE("ExptTcl::trialDescriptionCmd");
  if (objc != 1) {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }

  Tcl_SetObjResult(interp, Tcl_NewStringObj(getExpt().trialDescription(), -1));
  return TCL_OK;
}

//--------------------------------------------------------------------
//
// ExptTcl::unbindToglProc --
//
//
//
// Results:
//
//
//
// Side effects:
//
//
//
//--------------------------------------------------------------------

int ExptTcl::unbindToglProc(Tcl_Interp* interp) {
DOTRACE("ExptTcl::unbindToglProc");
  static Tcl_Obj* unbindCmdObj = 
    Tcl_NewStringObj("bind .togl <KeyPress> {}", -1);
  static TclObjLock lock_(unbindCmdObj);
  return Tcl_EvalObjEx(interp, unbindCmdObj, TCL_EVAL_GLOBAL);
}

//--------------------------------------------------------------------
//
// ExptTcl::undrawTrialCmd --
//
// Erase the current trial from the screen
//
// Results:
// none
//
// Side effects:
// The current trial is erased from the screen, and the Tlist's
// visibility is set to false, so that the trial does not reappear if
// any redraw events are sent to the screen window.
//
//--------------------------------------------------------------------

#ifdef LOCAL_TEST
int ExptTcl::undrawTrialCmd(ClientData, Tcl_Interp* interp,
                            int objc, Tcl_Obj* const objv[]) {
DOTRACE("ExptTcl::undrawTrialCmd");
  if (objc > 1) {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }

  getExpt().undrawTrial();
  return TCL_OK;
}
#endif

//--------------------------------------------------------------------
//
// ExptTcl::updateRegexpsProc --
//
//
//
// Results:
//
//
//
// Side effects:
//
//
//
//--------------------------------------------------------------------

int ExptTcl::updateRegexpsProc(Tcl_Interp* interp) {
DOTRACE("ExptTcl::updateRegexpsProc");

  theRegexps.clear();

  Tcl_Obj* key_resp_pairsObj = 
    Tcl_NewStringObj(getExpt().getKeyRespPairs().c_str(), -1);

  Tcl_Obj** pairsObjs;
  int objc;
  if (Tcl_ListObjGetElements(interp, key_resp_pairsObj, &objc, &pairsObjs)
      != TCL_OK)
    return TCL_ERROR;

  for (int i = 0; i < objc; i++) {
    Tcl_Obj* pairObj = pairsObjs[i];

    // Check that the length of the "pair" is 2
    int length;
    if (Tcl_ListObjLength(interp, pairObj, &length) != TCL_OK) return TCL_ERROR;
    if (length != 2) return TCL_ERROR;

    // Pull the regexp and the response value out of the list into
    // separate Tcl_Obj*'s
    Tcl_Obj *regexpObj, *response_valObj;
    if (Tcl_ListObjIndex(interp, pairObj, 0, &regexpObj) != TCL_OK)
      return TCL_ERROR;
    if (Tcl_ListObjIndex(interp, pairObj, 1, &response_valObj) != TCL_OK)
      return TCL_ERROR;

    // Retrieve a Tcl_RegExp and and int from
    Tcl_RegExp regexp = Tcl_GetRegExpFromObj(interp, regexpObj, 0);
    if (!regexp) return TCL_ERROR;

    int response_val;
    if (Tcl_GetIntFromObj(interp, response_valObj, &response_val) != TCL_OK)
      return TCL_ERROR;
    
    // Push the pair onto the vector
    theRegexps.push_back(RegExp_ResponseVal(regexp, response_val));
  }

  return TCL_OK;
}

//--------------------------------------------------------------------
//
// ExptTcl::useFeedbackCmd --
//
// Retrieve the current value or set a new value for whether feedback
// is given in the current Expt. If feedback is turned on, then a
// user-defined function named "feedback" must be defined to take a
// single argument (the trial response). This function will be called
// upon each successful trial completion, and the function should
// provide feedback as to whether the response was correct or
// incorrect. (However, note that the response does not need to deal
// with *invalid* responses; these are filtered out before "feedback"
// is called.)
//
// Results: 
// If the current value is being retrieved, returns a boolean
// indicating whether feedback is used.
//
// Side effects:
// If a new value is provided, feedback may be turned on or off.
//
//--------------------------------------------------------------------

int ExptTcl::useFeedbackCmd(ClientData, Tcl_Interp *interp,
                          int objc, Tcl_Obj *const objv[]) {
DOTRACE("ExptTcl::useFeedbackCmd");
  if (objc != 1 && objc != 2) {
    Tcl_WrongNumArgs(interp, 1, objv, "?new_value?");
    return TCL_ERROR;
  }

  if (objc == 1) {
    Tcl_SetObjResult(interp, Tcl_NewBooleanObj(getExpt().getUseFeedback()));
  }
  else if (objc == 2) {
    int val;
    if (Tcl_GetBooleanFromObj(interp, objv[1], &val) != TCL_OK)
      return TCL_ERROR;
    getExpt().setUseFeedback(val);
  }

  return TCL_OK;
}

//--------------------------------------------------------------------
//
// ExptTcl::verboseCmd --
//
//
//
// Results:
//
//
//
// Side effects:
//
//
//
//--------------------------------------------------------------------

int ExptTcl::verboseCmd(ClientData, Tcl_Interp *interp,
                        int objc, Tcl_Obj *const objv[]) {
DOTRACE("ExptTcl::verboseCmd");
  if (objc != 1 && objc != 2) {
    Tcl_WrongNumArgs(interp, 1, objv, "?new_value?");
    return TCL_ERROR;
  }

  if (objc == 1) {
    Tcl_SetObjResult(interp, Tcl_NewBooleanObj(getExpt().getVerbose()));
  }
  else if (objc == 2) {
    int val;
    if (Tcl_GetBooleanFromObj(interp, objv[1], &val) != TCL_OK)
      return TCL_ERROR;
    getExpt().setVerbose(val);
  }

  return TCL_OK;
}

//--------------------------------------------------------------------
//
// ExptTcl::writeCmd --
//
// Save the current state of the current Expt to file.
//
// Results:
// Returns an error if the filename is not valid, if the file is not
// writeable, or if a logic error occurs while the write is in progress.
//
// Side effects:
// none.
//
//--------------------------------------------------------------------

int ExptTcl::writeCmd(ClientData, Tcl_Interp* interp,
                      int objc, Tcl_Obj* const objv[]) {
DOTRACE("ExptTcl::writeCmd");
  if (objc != 2) {
    Tcl_WrongNumArgs(interp, 1, objv, "filename");
    return TCL_ERROR;
  }
  
  const char* filename = Tcl_GetString(objv[1]);
  Assert(filename);
  return getExptDriver(interp).writeProc(filename);
}

//--------------------------------------------------------------------
//
// ExptTcl::writeProc --
//
//
//
// Results:
//
//
//
// Side effects:
//
//
//
//--------------------------------------------------------------------

int ExptDriver::writeProc(const char* filename) {
DOTRACE("ExptDriver::writeProc");
  try {
    ofstream ofs(filename);
    if (ofs.fail()) throw IoFilenameError(filename);
    itsExpt.serialize(ofs, IO::BASES|IO::TYPENAME);
  }
  catch (IoError& err) {
	 err_message(itsInterp, "ExptDriver::writeProc", err.info().c_str());
    return TCL_ERROR;
  }
  return TCL_OK;
}

//--------------------------------------------------------------------
//
// Expt::write_and_exitcmd --
//
// Calls the helper function write_and_exitProc. See that function's
// comments for more details.
//
//--------------------------------------------------------------------

int ExptTcl::write_and_exitCmd(ClientData, Tcl_Interp *interp,
                               int objc, Tcl_Obj *const objv[]) {
DOTRACE("ExptTcl::write_and_exitCmd");
  if (objc != 1) {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }
  return getExptDriver(interp).write_and_exitProc();
}

//--------------------------------------------------------------------
//
// ExptTcl::write_and_exitProc --
//
//
//
// Results:
//
//
//
// Side effects:
//
//
//
//--------------------------------------------------------------------

int ExptDriver::write_and_exitProc() {
DOTRACE("ExptDriver::write_and_exitProc");
  int result = TCL_OK;

  // Get the current date/time and record it as the ending date for
  // thec current Expt.
  string date = getDateStringProc(itsInterp);
  if (!date.size()) return TCL_ERROR;
  itsExpt.setEndDate(date);

  // Parse the ending date for the current Expt and format into a
  // unique filename extension
  static Tcl_Obj* uniqueFilenameCmdObj = 
    Tcl_NewStringObj("clock format [clock scan [Expt::endDate]] "
                     "-format %H%M%d%b%Y", -1);
  static TclObjLock lock_(uniqueFilenameCmdObj);

  result = Tcl_EvalObjEx(itsInterp, uniqueFilenameCmdObj, TCL_EVAL_GLOBAL);
  if (result != TCL_OK) return result;
  string unique_filename = Tcl_GetStringResult(itsInterp);

  // Write the main experiment file
  string expt_filename = string("expt") + unique_filename;

  result = writeProc(expt_filename.c_str());
  if (result != TCL_OK) return result;

  cout << "wrote file " << expt_filename << endl;

  // Write the responses file
  string resp_filename = string("resp") + unique_filename;

  result = TlistTcl::write_responsesProc(itsInterp, NULL, resp_filename.c_str());
  if (result != TCL_OK) return result;

  cout << "wrote file " << resp_filename << endl;

  // Gracefully exit the application
  Tcl_Exit(0);

  return result;
}

//--------------------------------------------------------------------
//
// ExptTcl::Expt_Init --
//
// Create the Tcl procedures that make up the ExptTcl package, and
// link Expt variables into Tcl so they may be modified there. All
// procedures are added into the Expt:: namespace.
//
// Results:
// none.
//
// Side effects:
// none.
//
//--------------------------------------------------------------------

struct CmdName_CmdProc {
  const char* cmdName;
  Tcl_ObjCmdProc* cmdProc;
};

int ExptTcl::Expt_Init(Tcl_Interp* interp) {
DOTRACE("ExptTcl::Expt_Init");
  static CmdName_CmdProc Names_Procs[] = {
    { "Expt::abortWait", intVarCmd },
    { "Expt::autosaveFile", stringVarCmd },
    { "Expt::autosavePeriod", intVarCmd },
    { "Expt::begin", beginCmd },
    { "Expt::currentStimclass", currentStimclassCmd },
    { "Expt::currentTrial", currentTrialCmd },
    { "Expt::endDate", stringVarCmd },
    { "Expt::handleResponse", handleResponseCmd },
    { "Expt::init", initCmd },
    { "Expt::interTrialInterval", intVarCmd },
    { "Expt::isComplete", isCompleteCmd },
    { "Expt::keyRespPairs", stringVarCmd },
    { "Expt::numTrials", numTrialsCmd },
    { "Expt::numCompleted", numCompletedCmd },
    { "Expt::pause", pauseCmd },
    { "Expt::prevResponse", prevResponseCmd },
    { "Expt::read", readCmd },
    { "Expt::stimDur", intVarCmd },
    { "Expt::stop", stopCmd },
    { "Expt::timeout", intVarCmd },
    { "Expt::trialDescription", trialDescriptionCmd },
    { "Expt::useFeedback", useFeedbackCmd, },
    { "Expt::verbose", verboseCmd },
    { "Expt::write", writeCmd },
    { "Expt::write_and_exit", write_and_exitCmd }
#ifdef LOCAL_TEST
	 ,
    { "Expt::abortTrial", abortTrialCmd },
    { "Expt::beginTrial", beginTrialCmd },
    { "Expt::recordResponse", recordResponseCmd },
    { "Expt::undrawTrial", undrawTrialCmd }
#endif
  };

  // Add all commands to the ::Expt namespace.
  for (int i = 0; i < sizeof(Names_Procs)/sizeof(CmdName_CmdProc); i++) {
    Tcl_CreateObjCommand(interp, 
                         const_cast<char *>(Names_Procs[i].cmdName),
                         Names_Procs[i].cmdProc,
                         (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  }

  Tcl_LinkBoolean(interp, "Expt::haveTest", &haveTest, TCL_LINK_READ_ONLY);
  
  Tcl_PkgProvide(interp, "Expt", "2.1");
  return TCL_OK;
}

static const char vcid_expttcl_cc[] = "$Header$";
#endif // !EXPTTCL_CC_DEFINED
