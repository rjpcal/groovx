///////////////////////////////////////////////////////////////////////
//
// expttcl.cc
// Rob Peters
// created: Mon Mar  8 03:18:40 1999
// written: Fri Jun 11 12:00:35 1999
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
#include <cstring>

#include "tclobjlock.h"
#include "exptdriver.h"
#include "objlisttcl.h"
#include "tlist.h"
#include "expt.h"
#include "errmsg.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

///////////////////////////////////////////////////////////////////////
//
// Expt Tcl package declarations
//
///////////////////////////////////////////////////////////////////////

namespace ExptTcl {
  ExptDriver& exptDriver = ExptDriver::theExptDriver();
  ExptDriver& getExptDriver();

  // Tcl command procedures
  Tcl_ObjCmdProc beginCmd;
  Tcl_ObjCmdProc currentTrialTypeCmd;
  Tcl_ObjCmdProc currentTrialCmd;
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
  Tcl_ObjCmdProc undoPrevTrialCmd;
  Tcl_ObjCmdProc useFeedbackCmd;
  Tcl_ObjCmdProc verboseCmd;
  Tcl_ObjCmdProc writeCmd;
  Tcl_ObjCmdProc writeAndExitCmd;

  // OS/Tcl provided helpers
  string getDateStringProc(Tcl_Interp* interp);
  string getSubjectKeyProc(Tcl_Interp* interp);
};

///////////////////////////////////////////////////////////////////////
//
// Expt Tcl definitions
//
///////////////////////////////////////////////////////////////////////

inline ExptDriver& ExptTcl::getExptDriver() {
DOTRACE("ExptTcl::getExptDriver");
  return exptDriver;
}

Expt& ExptTcl::getExpt() {
DOTRACE("ExptTcl::getExpt");
  return exptDriver.expt();
}

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
// edBeginTrial is called, which displays a trial, and generates the 
// timer callbacks associated with a trial.
//
//--------------------------------------------------------------------

int ExptTcl::beginCmd(ClientData, Tcl_Interp *interp,
                      int objc, Tcl_Obj* const objv[]) {
DOTRACE("ExptTcl::beginCmd");
  if (objc != 1) {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }

  int result = TCL_OK;

  // Create the quit key binding
  static Tcl_Obj* bindQuitCmdObj = 
    Tcl_NewStringObj("bind .togl <KeyPress-q> { Expt::writeAndExit }", -1);
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

  getExptDriver().edBeginTrial();

  return result;
}

//--------------------------------------------------------------------
//
// ExptTcl::currentTrialTypeCmd --
//
// Results:
// Returns the stimclass of the current trial in the current experiment.
//
// Side effects:
// none.
//
//--------------------------------------------------------------------

int ExptTcl::currentTrialTypeCmd(ClientData, Tcl_Interp* interp,
                                 int objc, Tcl_Obj* const objv[]) {
DOTRACE("ExptTcl::currentTrialTypeCmd");
  if (objc != 1) {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }

  Expt& expt = getExptDriver().expt();

  Tcl_SetObjResult(interp, Tcl_NewIntObj(expt.currentTrialType()));
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

  Expt& expt = getExptDriver().expt();

  Tcl_SetObjResult(interp, Tcl_NewIntObj(expt.currentTrial()));
  return TCL_OK;
}

//--------------------------------------------------------------------
//
// ExptTcl::getDateStringProc --
//
// Return a nice human-readable date string in some default format.
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

  ExptDriver& exptDriver = getExptDriver();

  exptDriver.init(repeat, rand_seed, date, host, subject);
  return TCL_OK;
}

//--------------------------------------------------------------------
//
// ExptTcl::intVarCmd --
//
// Get or set one of the integer attributes associated with ExptDriver.
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

  // The struct Int_Get_Set is used to access Expt get/set functions
  // that handle ints
  typedef int (ExptDriver::* PExpt_int_getter) () const;
  typedef void (ExptDriver::* PExpt_int_setter) (int);

  struct Int_Get_Set {
    const char* name;
    PExpt_int_getter getter;
    PExpt_int_setter setter;
  };

  Int_Get_Set isg_params[] = {
    { "abortWait", &ExptDriver::getAbortWait, &ExptDriver::setAbortWait },
    { "autosavePeriod", &ExptDriver::getAutosavePeriod, &ExptDriver::setAutosavePeriod },
    { "interTrialInterval", &ExptDriver::getInterTrialInterval, 
      &ExptDriver::setInterTrialInterval },
    { "stimDur", &ExptDriver::getStimDur, &ExptDriver::setStimDur },
    { "timeout", &ExptDriver::getTimeout, &ExptDriver::setTimeout },
    { NULL, NULL, NULL }
  };

  // Find the index of the param in isg_params[]
  int offset = sizeof(Int_Get_Set);
  int index;
  if (Tcl_GetIndexFromObjStruct(interp, param_obj,
                                reinterpret_cast<char **>(isg_params), offset,
                                "property", 0, &index) != TCL_OK)
    return TCL_ERROR;

  ExptDriver& exptDriver = getExptDriver();

  // Retrieve current value
  if (objc == 1) {
    int val = (exptDriver.*isg_params[index].getter) ();
    Tcl_SetObjResult(interp, Tcl_NewIntObj(val));
  }

  // Set new value
  else if (objc == 2) {
    int new_val;
    if (Tcl_GetIntFromObj(interp, objv[1], &new_val) != TCL_OK)
      return TCL_ERROR;
    (exptDriver.*isg_params[index].setter) (new_val);
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

  Expt& expt = getExptDriver().expt();

  Tcl_SetObjResult(interp, Tcl_NewBooleanObj(expt.isComplete()));
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

  Expt& expt = getExptDriver().expt();

  Tcl_SetObjResult(interp, Tcl_NewIntObj(expt.numCompleted()));
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

  Expt& expt = getExptDriver().expt();

  Tcl_SetObjResult(interp, Tcl_NewIntObj(expt.numTrials()));
  return TCL_OK;
}

//--------------------------------------------------------------------
//
// ExptTcl::pauseCmd --
//
// Tell the ExptDriver to halt the experiment, then pop up a pause
// window. When the user dismisses the window, the experiment will
// resume.
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

  ExptDriver& exptDriver = getExptDriver();
  exptDriver.edHaltExpt();

  static Tcl_Obj* pauseMsgCmdObj = 
    Tcl_NewStringObj("tk_messageBox -default ok -icon info "
                     "-title \"Pause\" -type ok "
                     "-message \"Experiment paused. Click OK to continue.\"",
                     -1);
  static TclObjLock lock_(pauseMsgCmdObj);

  result = Tcl_EvalObjEx(interp, pauseMsgCmdObj, TCL_EVAL_GLOBAL);
  if (result != TCL_OK) return result;

  exptDriver.edEndTrial();

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

  Expt& expt = getExptDriver().expt();

  Tcl_SetObjResult(interp, Tcl_NewIntObj(expt.prevResponse()));
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
    getExptDriver().deserialize(ifs, IO::BASES|IO::TYPENAME);
  }
  catch (IoError& err) {
    err_message(interp, objv, err.info().c_str());
    return TCL_ERROR;
  }
  return TCL_OK;
}

//--------------------------------------------------------------------
//
// ExptTcl::stopCmd --
//
// Tell the ExptDriver to halt the experiment.
//
//--------------------------------------------------------------------

int ExptTcl::stopCmd(ClientData, Tcl_Interp *interp,
                     int objc, Tcl_Obj *const objv[]) {
DOTRACE("ExptTcl::stopCmd");
  if (objc != 1) {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }

  getExptDriver().edHaltExpt();
  return TCL_OK;
}

//--------------------------------------------------------------------
//
// ExptTcl::stringVarCmd --
//
// Get or set one of the string attributes associated with ExptDriver.
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

  // The struct String_Get_Set is used to access Expt get/set
  // functions that handle strings
  typedef const string& (ExptDriver::* PExpt_string_getter) () const;
  typedef void (ExptDriver::* PExpt_string_setter) (const string&);

  struct String_Get_Set {
    const char* name;
    PExpt_string_getter getter;
    PExpt_string_setter setter;
  };

  String_Get_Set ssg_params[] = {
    { "endDate", &ExptDriver::getEndDate, &ExptDriver::setEndDate },
    { "autosaveFile", &ExptDriver::getAutosaveFile, &ExptDriver::setAutosaveFile },
    { "keyRespPairs", &ExptDriver::getKeyRespPairs, &ExptDriver::setKeyRespPairs },
    { NULL, NULL, NULL }
  };

  // Find the index of the param in ssg_params[]
  int offset = sizeof(String_Get_Set);
  int index;
  if (Tcl_GetIndexFromObjStruct(interp, param_obj,
                                reinterpret_cast<char **>(ssg_params), offset,
                                "property", 0, &index) != TCL_OK)
    return TCL_ERROR;

  ExptDriver& exptDriver = getExptDriver();

  // Retrieve current value
  if (objc == 1) {
    const string& str = (exptDriver.*ssg_params[index].getter) ();
    Tcl_SetObjResult(interp, Tcl_NewStringObj(str.c_str(),-1));
  }

  // Set new value
  else if (objc == 2) {
    (exptDriver.*ssg_params[index].setter) (Tcl_GetString(objv[1]));
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
// Returns a human(experimenter)-readable description of the current
// trial that shows the trial's id, the trial's type, the id's of the
// objects that are displayed in the trial, the categories of those
// objects, and the number of completed trials and number of total
// trials.
//
// Side effects:
// none.
//
//--------------------------------------------------------------------

int ExptTcl::trialDescriptionCmd(ClientData, Tcl_Interp* interp,
                                 int objc, Tcl_Obj* const objv[]) {
DOTRACE("ExptTcl::trialDescriptionCmd");
  if (objc != 1) {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }

  Expt& expt = getExptDriver().expt();

  Tcl_SetObjResult(interp, Tcl_NewStringObj(expt.trialDescription(), -1));
  return TCL_OK;
}

//--------------------------------------------------------------------
//
// ExptTcl::undoPrevTrialCmd --
//
// The state of the experiment is restored to what it was just prior
// to the beginning of the most recent successfully completed
// trial. Thus, the current trial is changed to its previous value,
// and the response to the most recently successfully completed trial
// is erased.
//
// Results:
// none.
//
//--------------------------------------------------------------------

int ExptTcl::undoPrevTrialCmd(ClientData, Tcl_Interp *interp,
										int objc, Tcl_Obj *const objv[]) {
DOTRACE("ExptTcl::undoPrevTrialCmd");
  if (objc != 1) {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }

  getExptDriver().expt().undoPrevTrial();

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

  ExptDriver& exptDriver = getExptDriver();

  if (objc == 1) {
    Tcl_SetObjResult(interp, 
							Tcl_NewBooleanObj(exptDriver.getUseFeedback()));
  }
  else if (objc == 2) {
    int val;
    if (Tcl_GetBooleanFromObj(interp, objv[1], &val) != TCL_OK)
      return TCL_ERROR;
    exptDriver.setUseFeedback(val);
  }

  return TCL_OK;
}

//--------------------------------------------------------------------
//
// ExptTcl::verboseCmd --
//
// Get or set the value of ExptDriver's 'verbose' attribute.
//
//--------------------------------------------------------------------

int ExptTcl::verboseCmd(ClientData, Tcl_Interp *interp,
                        int objc, Tcl_Obj *const objv[]) {
DOTRACE("ExptTcl::verboseCmd");
  if (objc != 1 && objc != 2) {
    Tcl_WrongNumArgs(interp, 1, objv, "?new_value?");
    return TCL_ERROR;
  }

  ExptDriver& exptDriver = getExptDriver();

  if (objc == 1) {
    Tcl_SetObjResult(interp, Tcl_NewBooleanObj(exptDriver.getVerbose()));
  }
  else if (objc == 2) {
    int val;
    if (Tcl_GetBooleanFromObj(interp, objv[1], &val) != TCL_OK)
      return TCL_ERROR;
    exptDriver.setVerbose(val);
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

  return getExptDriver().write(filename);
}

//--------------------------------------------------------------------
//
// Expt::writeAndExitcmd --
//
// Calls the ExptDriver's function writeAndExit. See that function's
// comments for more details. Since that function calls Tcl_Exit(), it
// will never return, and therefore this function will also never
// return.
//
//--------------------------------------------------------------------

int ExptTcl::writeAndExitCmd(ClientData, Tcl_Interp *interp,
									  int objc, Tcl_Obj *const objv[]) {
DOTRACE("ExptTcl::writeAndExitCmd");
  if (objc != 1) {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }

  return getExptDriver().writeAndExit();
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

int ExptTcl::Expt_Init(Tcl_Interp* interp) {
DOTRACE("ExptTcl::Expt_Init");

  struct CmdName_CmdProc {
	 const char* cmdName;
	 Tcl_ObjCmdProc* cmdProc;
  };

  static CmdName_CmdProc Names_Procs[] = {
    { "Expt::abortWait", intVarCmd },
    { "Expt::autosaveFile", stringVarCmd },
    { "Expt::autosavePeriod", intVarCmd },
    { "Expt::begin", beginCmd },
    { "Expt::currentTrialType", currentTrialTypeCmd },
    { "Expt::currentTrial", currentTrialCmd },
    { "Expt::endDate", stringVarCmd },
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
    { "Expt::undoPrevTrial", undoPrevTrialCmd },
    { "Expt::useFeedback", useFeedbackCmd, },
    { "Expt::verbose", verboseCmd },
    { "Expt::write", writeCmd },
    { "Expt::writeAndExit", writeAndExitCmd }
  };

  ExptDriver::theExptDriver().setInterp(interp);

  // Add all commands to the ::Expt namespace.
  for (int i = 0; i < sizeof(Names_Procs)/sizeof(CmdName_CmdProc); ++i) {
    Tcl_CreateObjCommand(interp, 
                         const_cast<char *>(Names_Procs[i].cmdName),
                         Names_Procs[i].cmdProc,
								 (ClientData) NULL,
								 (Tcl_CmdDeleteProc *) NULL);
  }

  Tcl_PkgProvide(interp, "Expt", "2.1");
  return TCL_OK;
}

static const char vcid_expttcl_cc[] = "$Header$";
#endif // !EXPTTCL_CC_DEFINED
