///////////////////////////////////////////////////////////////////////
// exptdriver.cc
// Rob Peters
// created: Tue May 11 13:33:50 1999
// written: Tue May 11 14:23:10 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef EXPTDRIVER_CC_DEFINED
#define EXPTDRIVER_CC_DEFINED

#include "exptdriver.h"

#include <iostream.h>
#include <fstream.h>
#include <string>

#include "expt.h"
#include "tclobjlock.h"
#include "soundtcl.h"
#include "errmsg.h"
#include "tlisttcl.h"

#include "trace.h"
#include "debug.h"

namespace {

  string getDateStringProc(Tcl_Interp* interp) {
  DOTRACE("getDateStringProc");
    static Tcl_Obj* dateStringCmdObj =
		Tcl_NewStringObj("clock format [clock seconds]", -1);
	 static TclObjLock lock_(dateStringCmdObj);
	 
	 int result = Tcl_EvalObjEx(interp, dateStringCmdObj, TCL_EVAL_GLOBAL);
	 if (result != TCL_OK) return string();
	 
	 return string(Tcl_GetStringResult(interp));
  }

};

///////////////////////////////////////////////////////////////////////
//
// ExptTimer method definitions
//
///////////////////////////////////////////////////////////////////////

void ExptTimer::schedule(int msec, Tcl_Interp* interp) {
DOTRACE("ExptTimer::schedule");
  cancel();
  itsExptDriver.setInterp(interp);
  itsToken = Tcl_CreateTimerHandler(msec, dummyTimerProc, 
												static_cast<ClientData>(this));
}

void AbortTrialTimer::invoke() {
DOTRACE("AbortTrialTimer::invoke");
  itsExptDriver.abortTrial();
}

void StartTrialTimer::invoke() {
DOTRACE("StartTrialTimer::invoke");
  itsExptDriver.startTrial();
}

void UndrawTrialTimer::invoke() {
DOTRACE("UndrawTrialTimer::invoke");
  itsExptDriver.expt().undrawTrial();
}

///////////////////////////////////////////////////////////////////////
//
// ExptDriver method definitions
//
///////////////////////////////////////////////////////////////////////

ExptDriver::ExptDriver(Expt& expt) : 
  itsExpt(expt), itsInterp(NULL),
  itsAbortTimer(*this),
  itsStartTimer(*this),
  itsUndrawTrialTimer(*this)
{
DOTRACE("ExptDriver::ExptDriver");
}

//--------------------------------------------------------------------
//
// ExptDriver::abortTrial --
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

  result = ignoreResponse();
  Assert(result == TCL_OK);

  result = SoundTcl::playProc(itsInterp, NULL, "err");
  Assert(result == TCL_OK);

  itsExpt.abortTrial();

  itsStartTimer.schedule(itsExpt.getAbortWait(), itsInterp);
}

//--------------------------------------------------------------------
//
// ExptDriver::attendResponse --
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

int ExptDriver::attendResponse() {
DOTRACE("ExptDriver::attendResponse");
  static Tcl_Obj* bindCmdObj = 
    Tcl_NewStringObj("bind .togl <KeyPress> { Expt::handleResponse %K }", -1);
  static TclObjLock lock_(bindCmdObj);
  return Tcl_EvalObjEx(itsInterp, bindCmdObj, TCL_EVAL_GLOBAL);
}

//--------------------------------------------------------------------
//
// ExptDriver::feedbackProc --
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

int ExptDriver::feedbackProc(int response) {
DOTRACE("ExptDriver::feedbackProc");
  static Tcl_Obj* feedbackObj = 
    Tcl_NewStringObj("::feedback", -1);
  static TclObjLock lock_(feedbackObj);

  static Tcl_Obj* feedbackObjv[2] = { feedbackObj, NULL };

  feedbackObjv[1] = Tcl_NewIntObj(response);
  TclObjLock lock2_(feedbackObjv[1]);

  return Tcl_EvalObjv(itsInterp, 2, feedbackObjv, TCL_EVAL_GLOBAL);
}

//--------------------------------------------------------------------
//
// pkg_name::cmd_nameCmd --
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

int ExptDriver::handleResponse(const char* keysym) {
DOTRACE("ExptDriver::handleResponse");

  int result = TCL_OK;

  result = ignoreResponse();
  if (result != TCL_OK) return result;

  ExptDriver::stopTime();

  itsExpt.undrawTrial();

  // Get exactly one character -- the last character -- in keysym
  // (just before the terminating '\0'). This is the character that we
  // will take as the response. (The last character must be extracted
  // to handle the numeric keypad, where the keysysms are 'KP_0',
  // 'KP_3', etc., and we want just the 0 or the 3).
  const char* last_char = keysym;
  while ( *(last_char+1) != '\0' ) { last_char++; }
    
  int resp = -1;
  for (int i = 0; i < itsRegexps.size(); i++) {
    int found = Tcl_RegExpExec(itsInterp, itsRegexps[i].regexp, 
                               last_char, last_char);

	 // Return value of -1 indicates error while executing in the
	 // regular expression
    if (found == -1) return TCL_ERROR;

	 // Return value of 0 indicates no match was found, so do nothing
	 // and go to the next loop cycle
	 if (found == 0) continue;

	 // Return value of 1 indicates a match was found, so record the
	 // response value that corresponds with the regexp and exit the loop
    if (found == 1) {
      resp = itsRegexps[i].resp_val;
      break;
    }
  }
    
  // If resp is -1, then the keypress did not match any of the regexps
  // in theRegexps, so the response was invalid, and we abort the
  // trial.
  if (resp == -1) {
    ExptDriver::abortTrial();
    return TCL_OK;
  }

  // If useFeedback is true, we call the user-defined feedback
  // function with resp as its argument.
  if (itsExpt.getUseFeedback()) {
    result = feedbackProc(resp);
    if (result != TCL_OK) return result;
  }

  itsExpt.recordResponse(resp);

  if (itsExpt.getVerbose()) {
    cout << "response " << resp << endl;
  }

  if (itsExpt.isComplete()) {
    cout << "expt complete" << endl;
    return ExptDriver::write_and_exitProc();
  }    
  
  scheduleNextTrial();

  return result;
}

//--------------------------------------------------------------------
//
// pkg_name::cmd_nameCmd --
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

void ExptDriver::scheduleNextTrial() {
DOTRACE("ExptDriver::scheduleNextTrial");
  cerr << "ExptDriver::scheduleNextTrial" << endl;
  itsStartTimer.schedule(itsExpt.getInterTrialInterval(), itsInterp);
}

//--------------------------------------------------------------------
//
// ExptDriver::startTrial --
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
  
  itsUndrawTrialTimer.cancel();
  itsAbortTimer.cancel();

  if ( itsExpt.needAutosave() ) {
	 result = writeProc(itsExpt.getAutosaveFile().c_str());
    Assert(result == TCL_OK);
  }

  // Clear the event queue before we rebind to KeyPress events
  while (Tcl_DoOneEvent(TCL_ALL_EVENTS|TCL_DONT_WAIT) != 0) {
    ; // Empty loop body
  }

  result = attendResponse();
  Assert(result == TCL_OK);

  itsExpt.beginTrial();

  itsUndrawTrialTimer.schedule(itsExpt.getStimDur(), itsInterp);
  itsAbortTimer.schedule(itsExpt.getTimeout(), itsInterp);

  if (itsExpt.getVerbose()) {
    cerr << itsExpt.trialDescription() << endl;
  }
}

//--------------------------------------------------------------------
//
// ExptDriver::stopTime --
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

int ExptDriver::stopTime() {
DOTRACE("ExptDriver::stopTime");
  itsAbortTimer.cancel();
  itsStartTimer.cancel();
  itsUndrawTrialTimer.cancel();

  int result = TCL_OK;
  result = ignoreResponse();
  if (result != TCL_OK) return result;

  itsExpt.undrawTrial();
  itsExpt.abortTrial();
  return TCL_OK;
}

//--------------------------------------------------------------------
//
// ExptDriver::ignoreResponse --
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

int ExptDriver::ignoreResponse() {
DOTRACE("ExptDriver::ignoreResponse");
  static Tcl_Obj* unbindCmdObj = 
    Tcl_NewStringObj("bind .togl <KeyPress> {}", -1);
  static TclObjLock lock_(unbindCmdObj);
  return Tcl_EvalObjEx(itsInterp, unbindCmdObj, TCL_EVAL_GLOBAL);
}

//--------------------------------------------------------------------
//
// ExptDriver::updateRegexpsProc --
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

int ExptDriver::updateRegexpsProc() {
DOTRACE("ExptDriver::updateRegexpsProc");

  itsRegexps.clear();

  Tcl_Obj* key_resp_pairsObj = 
    Tcl_NewStringObj(itsExpt.getKeyRespPairs().c_str(), -1);

  Tcl_Obj** pairsObjs;
  int objc;
  if (Tcl_ListObjGetElements(itsInterp, key_resp_pairsObj, &objc, &pairsObjs)
      != TCL_OK)
    return TCL_ERROR;

  for (int i = 0; i < objc; i++) {
    Tcl_Obj* pairObj = pairsObjs[i];

    // Check that the length of the "pair" is really 2
    int length;
    if (Tcl_ListObjLength(itsInterp, pairObj, &length) != TCL_OK) 
		return TCL_ERROR;
    if (length != 2) return TCL_ERROR;

    // Pull the regexp and the response value out of the list into
    // separate Tcl_Obj*'s
    Tcl_Obj *regexpObj, *response_valObj;
    if (Tcl_ListObjIndex(itsInterp, pairObj, 0, &regexpObj) != TCL_OK)
      return TCL_ERROR;
    if (Tcl_ListObjIndex(itsInterp, pairObj, 1, &response_valObj) != TCL_OK)
      return TCL_ERROR;

    // Retrieve a Tcl_RegExp and and int from
    Tcl_RegExp regexp = Tcl_GetRegExpFromObj(itsInterp, regexpObj, 0);
    if (!regexp) return TCL_ERROR;

    int response_val;
    if (Tcl_GetIntFromObj(itsInterp, response_valObj, &response_val) != TCL_OK)
      return TCL_ERROR;
    
    // Push the pair onto the vector
    itsRegexps.push_back(RegExp_ResponseVal(regexp, response_val));
  }

  return TCL_OK;
}

//--------------------------------------------------------------------
//
// ExptDriver::writeProc --
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
// ExptDriver::write_and_exitProc --
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

static const char vcid_exptdriver_cc[] = "$Header$";
#endif // !EXPTDRIVER_CC_DEFINED
