///////////////////////////////////////////////////////////////////////
// expttcl.cc
// Rob Peters
// created: Mon Mar  8 03:18:40 1999
// written: Mon May  3 13:00:46 1999
// $Id$
//
// This file defines the procedures that provide the Tcl interface to
// the Expt class, and defines several Tcl variables that are provided
// within the Tcl Expt:: namespace.
///////////////////////////////////////////////////////////////////////

#ifndef EXPTTCL_CC_DEFINED
#define EXPTTCL_CC_DEFINED

#include "expttcl.h"

#include <iostream.h>
#include <fstream.h>
#include <tcl.h>
#include <cstring>

#include "objlisttcl.h"
#include "tlisttcl.h"
#include "expt.h"
#include "errmsg.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

///////////////////////////////////////////////////////////////////////
// Expt Tcl package declarations
///////////////////////////////////////////////////////////////////////

namespace ExptTcl {

  Tcl_ObjCmdProc begin_exptCmd;
  Tcl_ObjCmdProc pauseCmd;
  Tcl_ObjCmdProc stopCmd;
  Tcl_ObjCmdProc start_trialCmd;
  Tcl_ObjCmdProc abortCmd;
  Tcl_ObjCmdProc KeyBindingCmd;
//   Tcl_ObjCmdProc UnbindKeyPressCmd;
//   Tcl_ObjCmdProc BindKeyPressCmd;
  Tcl_ObjCmdProc write_and_exitCmd;

  // Tcl command procedures
  Tcl_ObjCmdProc abortTrialCmd;
  Tcl_ObjCmdProc beginTrialCmd;
  Tcl_ObjCmdProc currentStimclassCmd;
  Tcl_ObjCmdProc currentTrialCmd;
  Tcl_ObjCmdProc initCmd; 
  Tcl_ObjCmdProc isCompleteCmd;
  Tcl_ObjCmdProc numTrialsCmd;
  Tcl_ObjCmdProc numCompletedCmd;
  Tcl_ObjCmdProc prevResponseCmd;
  Tcl_ObjCmdProc readCmd; 
  Tcl_ObjCmdProc recordResponseCmd;
  Tcl_ObjCmdProc trialDescriptionCmd;
  Tcl_ObjCmdProc undrawTrialCmd;
  Tcl_ObjCmdProc writeCmd;

  const char* const pathname = ".togl";
  Tcl_Obj* const pathnameObj = Tcl_NewStringObj(pathname, -1);

  Tcl_TimerToken startTimer=NULL;
  void startProc(ClientData clientData) {
	 Tcl_Interp* interp = static_cast<Tcl_Interp *>(clientData);
	 Tcl_Eval(interp, "start_trial");
	 cerr << "startProc\n";
  }

  Tcl_TimerToken undrawTrialTimer=NULL;
  void undrawTrialProc(ClientData) {
	 cerr << "undrawTrialProc\n";
	 getExpt().undrawTrial();
  }

  Tcl_TimerToken abortTimer=NULL;
  void abortProc(ClientData clientData) {
	 Tcl_Interp* interp = static_cast<Tcl_Interp *>(clientData);
	 Tcl_Eval(interp, "abort");
	 cerr << "abortProc\n";
  }

  Tcl_TimerToken resumeTimer=NULL;
  void resumeProc(ClientData clientData) {
	 Tcl_Interp* interp = static_cast<Tcl_Interp *>(clientData);
	 Tcl_Eval(interp, "start_trial");
	 cerr << "resumeProc\n";
  }

  int inter_trial_interval=1000;

  // Variables that will be linked to Tcl variables
  int abort_wait=1000;
  int timeout=4000;
  int stim_dur=2000;
  int verbose=0;
  int feedback=0;
  char* date=NULL;
}

///////////////////////////////////////////////////////////////////////
// Helper functions that provide typesafe access to Tcl_LinkVar
///////////////////////////////////////////////////////////////////////

namespace {

  inline int Tcl_LinkInt(Tcl_Interp* interp, const char* varName, 
								 int* addr, int flag) {
	 auto_ptr<char> temp(new char[strlen(varName) + 1]);
	 strcpy(temp.get(), varName);
	 flag &= TCL_LINK_READ_ONLY;
	 return Tcl_LinkVar(interp, temp.get(), reinterpret_cast<char *>(addr),
							  flag | TCL_LINK_INT);
  }
  inline int Tcl_LinkDouble(Tcl_Interp* interp, const char* varName, 
									 double* addr, int flag) {
	 auto_ptr<char> temp(new char[strlen(varName) + 1]);
	 strcpy(temp.get(), varName);
	 flag &= TCL_LINK_READ_ONLY;
	 return Tcl_LinkVar(interp, temp.get(), reinterpret_cast<char *>(addr),
							  flag | TCL_LINK_DOUBLE);
  }
  inline int Tcl_LinkBoolean(Tcl_Interp* interp, const char* varName, 
									  int* addr, int flag) {
	 auto_ptr<char> temp(new char[strlen(varName) + 1]);
	 strcpy(temp.get(), varName);
	 flag &= TCL_LINK_READ_ONLY;
	 return Tcl_LinkVar(interp, temp.get(), reinterpret_cast<char *>(addr),
							  flag | TCL_LINK_BOOLEAN);
  }
  inline int Tcl_LinkString(Tcl_Interp* interp, const char* varName, 
									 char** addr, int flag) {
	 auto_ptr<char> temp(new char[strlen(varName) + 1]);
	 strcpy(temp.get(), varName);
	 flag &= TCL_LINK_READ_ONLY;
	 return Tcl_LinkVar(interp, temp.get(), reinterpret_cast<char *>(addr),
							  flag | TCL_LINK_STRING);
  }  
}

///////////////////////////////////////////////////////////////////////
// Procedures ported from Tcl
///////////////////////////////////////////////////////////////////////

int ExptTcl::write_and_exitCmd(ClientData, Tcl_Interp *interp,
										 int objc, Tcl_Obj *const objv[]) {
DOTRACE("ExptTcl::write_and_exitCmd");
  if (objc != 1) {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }

  int result = TCL_OK;

  result = Tcl_Eval(interp, "Expt::write \"expt$Expt::date\"");
  if (result != TCL_OK) return result;

  result = Tcl_Eval(interp, "puts \"wrote file expt$Expt::date\"");
  if (result != TCL_OK) return result;

  result = Tcl_Eval(interp, "Tlist::write_responses \"resp$Expt::date\"");
  if (result != TCL_OK) return result;

  result = Tcl_Eval(interp, "puts \"wrote file resp$Expt::date\"");
  if (result != TCL_OK) return result;

  result = Tcl_Eval(interp, "exit");

  return result;
}

// int ExptTcl::BindKeyPressCmd(ClientData, Tcl_Interp *interp,
// 									  int objc, Tcl_Obj *const objv[]) {
// DOTRACE("ExptTcl::BindKeyPressCmd");
//   if (objc != 1) {
//     Tcl_WrongNumArgs(interp, 1, objv, NULL);
//     return TCL_ERROR;
//   }

//   return Tcl_Eval(interp, "bind .togl <KeyPress> { KeyBinding %K }");
// }

// int ExptTcl::UnbindKeyPressCmd(ClientData, Tcl_Interp *interp,
// 										 int objc, Tcl_Obj *const objv[]) {
// DOTRACE("ExptTcl::UnbindKeyPressCmd");
//   if (objc != 1) {
//     Tcl_WrongNumArgs(interp, 1, objv, NULL);
//     return TCL_ERROR;
//   }

//   return Tcl_Eval(interp, "bind .togl <KeyPress> {}");
// }

int ExptTcl::KeyBindingCmd(ClientData, Tcl_Interp *interp,
									int objc, Tcl_Obj *const objv[]) {
DOTRACE("ExptTcl::KeyBindingCmd");
  if (objc != 2) {
    Tcl_WrongNumArgs(interp, 1, objv, "keysym");
    return TCL_ERROR;
  }

  Expt& expt = getExpt();

  // Name the second arg "keysym"
  Tcl_ObjSetVar2(interp, Tcl_NewStringObj("keysym", -1), NULL,
					  objv[1], TCL_LEAVE_ERR_MSG);

  int result = TCL_OK;

  result = Tcl_Eval(interp, "bind .togl <KeyPress> {}");
  if (result != TCL_OK) return result;

  Tcl_DeleteTimerHandler(undrawTrialTimer);
  //  result = Tcl_Eval(interp, "after cancel $Expt::CLEARSCREEN_ID");
  //  if (result != TCL_OK) return result;

  Tcl_DeleteTimerHandler(abortTimer);
//   result = Tcl_Eval(interp, "after cancel $Expt::ABORT_ID");
//   if (result != TCL_OK) return result;

  expt.undrawTrial();
//   result = Tcl_Eval(interp, "Expt::undrawTrial");
//   if (result != TCL_OK) return result;

	 
  // get exactly one character from keysym
  result = Tcl_Eval(interp, 
	 "set char [string index $keysym [expr [string length $keysym] - 1]]");
  if (result != TCL_OK) return result;

	 
  // We unset the variable `resp`, then compare char to each of the
  // keys in ::key_resp_pairs. If there is a match, resp is set to the
  // corresponding response value; otherwise, resp remains unset.
  result = Tcl_Eval(interp, "if { [info exists resp] } { unset resp }");
  if (result != TCL_OK) return result;

  result = Tcl_Eval(interp,			  
			  "foreach key_resp_pair $::key_resp_pairs {\n"
			  "  if { [regexp [lindex $key_resp_pair 0] $char] } {\n"
			  "    set resp [lindex $key_resp_pair 1]\n"
			  "    break\n"
			  "  }\n"
			  "}");
  if (result != TCL_OK) return result;

	 
  // If resp does not exist, then the keypress did not match any of
  // the keys in ::key_resp_pairs, so the response was invalid, and
  // we abort the trial.
  Tcl_Obj* resp_obj = Tcl_ObjGetVar2(interp, Tcl_NewStringObj("resp", -1),
												 NULL, TCL_LEAVE_ERR_MSG);
  if (!resp_obj) {
	 return Tcl_Eval(interp, "abort");
  }
  //   result = Tcl_Eval(interp, "if { ![info exists resp] } { abort; return; }");
	 
  result = Tcl_Eval(interp, "feedback $resp");
  if (result != TCL_OK) return result;
  
  int resp;
  if (Tcl_GetIntFromObj(interp, resp_obj, &resp) != TCL_OK) return TCL_ERROR;
  expt.recordResponse(resp);
//   result = Tcl_Eval(interp, "Expt::recordResponse $resp");
//   if (result != TCL_OK) return result;

  if (verbose) {
	 cout << "response " << resp << endl;
  }
//   result = Tcl_Eval(interp, "if { $Expt::verbose != 0 } { puts \"response $resp\" }");
//   if (result != TCL_OK) return result;

  if (expt.isComplete()) {
	 cout << "expt complete" << endl;
	 return Tcl_Eval(interp, "write_and_exit");
  }
//   result = Tcl_Eval(interp, 
// 	  "if { [Expt::isComplete] } { puts \"expt complete\"; write_and_exit; }");
//   if (result != TCL_OK) return result;
	 
  
  startTimer = Tcl_CreateTimerHandler(inter_trial_interval, startProc,
												  static_cast<ClientData>(interp));
//   result = Tcl_Eval(interp, "set Expt::START_ID [after 1000 start_trial]");
//   if (result != TCL_OK) return result;

  return result;
}

int ExptTcl::abortCmd(ClientData, Tcl_Interp *interp,
							 int objc, Tcl_Obj *const objv[]) {
DOTRACE("ExptTcl::abortCmd");
  if (objc != 1) {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }

  int result = TCL_OK;

  result = Tcl_Eval(interp, "bind .togl <KeyPress> {}");
  if (result != TCL_OK) return result;

  result = Tcl_Eval(interp, "Sound::play err");
  if (result != TCL_OK) return result;

  Expt& expt = getExpt();
  expt.abortTrial();
//   result = Tcl_Eval(interp, "Expt::abortTrial");
//   if (result != TCL_OK) return result;

  result = Tcl_Eval(interp, "after $Expt::abort_wait start_trial");
  if (result != TCL_OK) return result;

  return result;
}

int ExptTcl::start_trialCmd(ClientData, Tcl_Interp *interp,
									 int objc, Tcl_Obj *const objv[]) {
DOTRACE("ExptTcl::start_trialCmd");
  if (objc != 1) {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }

  int result = TCL_OK;
  
  Tcl_DeleteTimerHandler(undrawTrialTimer);
//   result = Tcl_Eval(interp, 
// 			  "if { [info exists Expt::CLEARSCREEN_ID] } {\n"
// 			  "after cancel $Expt::CLEARSCREEN_ID}");
//   if (result != TCL_OK) return result;

  Tcl_DeleteTimerHandler(abortTimer);
//   result = Tcl_Eval(interp, 
// 			  "if { [info exists Expt::ABORT_ID] } {\n"
// 			  "after cancel $Expt::ABORT_ID}");
//   if (result != TCL_OK) return result;

  result = Tcl_Eval(interp, "update");
  if (result != TCL_OK) return result;

  result = Tcl_Eval(interp, 
	 "if { [expr [Expt::numCompleted] % $Expt::autosave_period] == 0 } {\n"
			  "Expt::write $Expt::autosave_file\n"
	 "}");
  if (result != TCL_OK) return result;

  result = Tcl_Eval(interp, "bind .togl <KeyPress> { KeyBinding %K }");
  if (result != TCL_OK) return result;

  Expt& expt = getExpt();
  expt.beginTrial();
//   result = Tcl_Eval(interp, "Expt::beginTrial");
//   if (result != TCL_OK) return result;

  undrawTrialTimer = Tcl_CreateTimerHandler(stim_dur, undrawTrialProc, NULL);
//   result = Tcl_Eval(interp, 
// 	 "set Expt::CLEARSCREEN_ID [after $Expt::stim_dur Expt::undrawTrial]");
//   if (result != TCL_OK) return result;

  abortTimer = Tcl_CreateTimerHandler(timeout, abortProc, 
												  static_cast<ClientData>(interp));
//   result = Tcl_Eval(interp, 
//     "set Expt::ABORT_ID [after $Expt::timeout abort]");
//   if (result != TCL_OK) return result;

  if (verbose) {
	 cerr << expt.trialDescription() << endl;
  }
//   result = Tcl_Eval(interp, 
//     "if { $Expt::verbose != 0 } { puts [Expt::trialDescription] }");
//   if (result != TCL_OK) return result;

  return result;
}

int ExptTcl::stopCmd(ClientData, Tcl_Interp *interp,
							int objc, Tcl_Obj *const objv[]) {
DOTRACE("ExptTcl::stopCmd");
  if (objc != 1) {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }

  int result = TCL_OK;

  Tcl_DeleteTimerHandler(resumeTimer);
//   result = Tcl_Eval(interp, 
// 			  "if { [info exists Expt::RESUME_ID] } {\n"
// 			  "after cancel $Expt::RESUME_ID}");
//   if (result != TCL_OK) return result;

  Tcl_DeleteTimerHandler(undrawTrialTimer);
//   result = Tcl_Eval(interp, 
// 			  "if { [info exists Expt::CLEARSCREEN_ID] } {\n"
// 			  "after cancel $Expt::CLEARSCREEN_ID}");
//   if (result != TCL_OK) return result;

  Tcl_DeleteTimerHandler(abortTimer);
//   result = Tcl_Eval(interp, 
// 			  "if { [info exists Expt::ABORT_ID] } {\n"
// 			  "after cancel $Expt::ABORT_ID}");
//   if (result != TCL_OK) return result;

  result = Tcl_Eval(interp, "bind .togl <KeyPress> {}");
  if (result != TCL_OK) return result;

  Expt& expt = getExpt();
  expt.undrawTrial();
//   result = Tcl_Eval(interp, "Expt::undrawTrial");
//   if (result != TCL_OK) return result;

  expt.abortTrial();
//   result = Tcl_Eval(interp, "Expt::abortTrial");
//   if (result != TCL_OK) return result;

  return TCL_OK;
}

int ExptTcl::pauseCmd(ClientData, Tcl_Interp *interp,
							 int objc, Tcl_Obj *const objv[]) {
DOTRACE("ExptTcl::pauseCmd");
  if (objc != 1) {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }

  int result = TCL_OK;

  result = Tcl_Eval(interp, "stop");
  if (result != TCL_OK) return result;

  result = Tcl_Eval(interp, 
    "tk_messageBox -default ok -icon info -title \"Pause\" -type ok\
        -message \"Experiment paused. Click OK to continue.\"");
  if (result != TCL_OK) return result;

  resumeTimer = Tcl_CreateTimerHandler(inter_trial_interval, resumeProc,
													static_cast<ClientData>(interp));
//   result = Tcl_Eval(interp, "set Expt::RESUME_ID [after 1000 start_trial]");
//   if (result != TCL_OK) return result;

  return result;
}

int ExptTcl::begin_exptCmd(ClientData, Tcl_Interp *interp,
									int objc, Tcl_Obj *const objv[]) {
DOTRACE("ExptTcl::begin_exptCmd");
  if (objc != 1) {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }

  int result = TCL_OK;

  // How often autosave is performed, in # of trials
  result = Tcl_Eval(interp, "set Expt::autosave_period 10");

  // Filename to use for autosave data
  result = Tcl_Eval(interp, "set Expt::autosave_file __autosave_expt");

  result = Tcl_Eval(interp, "bind .togl <KeyPress-q> { write_and_exit }");
  if (result != TCL_OK) return result;

  result = Tcl_Eval(interp, "bind .togl <KeyPress-p> { pause }");
  if (result != TCL_OK) return result;

  result = Tcl_Eval(interp, "bind .togl <KeyPress-s> {}");
  if (result != TCL_OK) return result;

  result = Tcl_Eval(interp, "focus -force .togl");
  if (result != TCL_OK) return result;

  result = Tcl_Eval(interp, "start_trial");
  if (result != TCL_OK) return result;

  return result;
}

///////////////////////////////////////////////////////////////////////
// Expt Tcl definitions
///////////////////////////////////////////////////////////////////////

Expt& ExptTcl::getExpt() {
DOTRACE("ExptTcl::getExpt");
  static Expt theExpt(TlistTcl::getTlist(), 0);
  return theExpt;
}

//--------------------------------------------------------------------
// abortTrialCmd
//
// This command simply calls the abortTrial() method of the active
// Expt. This method in turn puts the current (to be aborted) trial at
// the end of the trial sequence, without recording any response. The
// next call to beginTrial will start the same trial as would have
// started if the current trial had been completed normally, instead
// of with abortTrial().
//--------------------------------------------------------------------
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

  Tcl_Eval(interp, "clock format [clock seconds]");
  string date = Tcl_GetString(Tcl_GetObjResult(interp));
  
  Tcl_Eval(interp, "exec hostname");
  string host = Tcl_GetString(Tcl_GetObjResult(interp));
  
  Tcl_Eval(interp, "file tail [pwd]");
  string subject = Tcl_GetString(Tcl_GetObjResult(interp));

  getExpt().init(repeat, rand_seed, date, host, subject);
  return TCL_OK;
}

int ExptTcl::isCompleteCmd(ClientData, Tcl_Interp* interp,
									int objc, Tcl_Obj* const objv[]) {
DOTRACE("ExptTcl::isCompleteCmd");
  if (objc != 1) {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }
  Tcl_SetObjResult(interp, Tcl_NewIntObj(getExpt().isComplete()));
  return TCL_OK;
}

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

int ExptTcl::recordResponseCmd(ClientData, Tcl_Interp* interp,
										 int objc, Tcl_Obj* const objv[]) {
DOTRACE("ExptTcl::recordResponseCmd");
  if (objc != 2 && objc != 3) {
    Tcl_WrongNumArgs(interp, 1, objv, "response [time_in_msec]");
    return TCL_ERROR;
  }

  int response;
  if (Tcl_GetIntFromObj(interp, objv[1], &response) != TCL_OK)
	 return TCL_ERROR;

  // Ignore the time_in_msec argument
  getExpt().recordResponse(response);
  return TCL_OK;
}

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

int ExptTcl::writeCmd(ClientData, Tcl_Interp* interp,
							 int objc, Tcl_Obj* const objv[]) {
DOTRACE("ExptTcl::writeCmd");
  if (objc != 2) {
    Tcl_WrongNumArgs(interp, 1, objv, "filename");
    return TCL_ERROR;
  }
  
  char* filename = Tcl_GetString(objv[1]);
  Assert(filename);

  try {
	 ofstream ofs(filename);
	 if (ofs.fail()) throw IoFilenameError(filename);
	 getExpt().serialize(ofs, IO::BASES|IO::TYPENAME);
  }
  catch (IoError& err) {
	 err_message(interp, objv, err.info().c_str());
	 return TCL_ERROR;
  }
  return TCL_OK;
}

int ExptTcl::Expt_Init(Tcl_Interp* interp) {
DOTRACE("ExptTcl::Expt_Init");
  Tcl_CreateObjCommand(interp, "write_and_exit", write_and_exitCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
//   Tcl_CreateObjCommand(interp, "BindKeyPress", BindKeyPressCmd,
//                        (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
//   Tcl_CreateObjCommand(interp, "UnbindKeyPress", UnbindKeyPressCmd,
//                        (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "KeyBinding", KeyBindingCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "abort", abortCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "start_trial", start_trialCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "stop", stopCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "pause", pauseCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "begin_expt", begin_exptCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);

  // All commands added to the ::Expt namespace.
  Tcl_CreateObjCommand(interp, "Expt::abortTrial", abortTrialCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "Expt::beginTrial", beginTrialCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "Expt::currentStimclass", currentStimclassCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "Expt::currentTrial", currentTrialCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "Expt::init", initCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "Expt::isComplete", isCompleteCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "Expt::numTrials", numTrialsCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "Expt::numCompleted", numCompletedCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "Expt::prevResponse", prevResponseCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "Expt::read", readCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "Expt::recordResponse", recordResponseCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "Expt::trialDescription", trialDescriptionCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "Expt::undrawTrial", undrawTrialCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "Expt::write", writeCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);

  Tcl_LinkInt(interp, "Expt::abort_wait", &abort_wait, 0);
  Tcl_LinkInt(interp, "Expt::timeout", &timeout, 0);
  Tcl_LinkInt(interp, "Expt::stim_dur", &stim_dur, 0);
  Tcl_LinkBoolean(interp, "Expt::verbose", &verbose, 0);
  Tcl_LinkBoolean(interp, "Expt::feedback", &feedback, 0);
  Tcl_LinkString(interp, "Expt::date", &date, 0);

  Tcl_PkgProvide(interp, "Expt", "2.1");
  return TCL_OK;
}

static const char vcid_expttcl_cc[] = "$Header$";
#endif // !EXPTTCL_CC_DEFINED
