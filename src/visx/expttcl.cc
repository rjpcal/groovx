///////////////////////////////////////////////////////////////////////
// expttcl.cc
// Rob Peters
// created: Mon Mar  8 03:18:40 1999
// written: Sun Apr 25 13:31:38 1999
// $Id$
//
// This file defines the procedures that provide the Tcl interface to
// the Expt class, and defines several Tcl variables that are provided
// within the Tcl Expt:: namespace.
///////////////////////////////////////////////////////////////////////

#ifndef EXPTTCL_CC_DEFINED
#define EXPTTCL_CC_DEFINED

#include "expttcl.h"

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
  Tcl_ObjCmdProc writeCmd;

  // Variables that will be linked to Tcl variables
  int clicks1=0;
  int clicks2=0;
  int clicks_per_ms=1000;
  int abort_wait=0;
  int timeout=0;
  int stim_dur=0;
  int verbose=0;
  int feedback=0;
  char* date=NULL;
}

///////////////////////////////////////////////////////////////////////
// Helper functions that provide typesafe access to Tcl_LinkVar
///////////////////////////////////////////////////////////////////////

namespace {

  inline int Tcl_LinkInt(Tcl_Interp *interp, const char* varName, 
								 int* addr, int flag) {
	 auto_ptr<char> temp(new char[strlen(varName) + 1]);
	 strcpy(temp.get(), varName);
	 flag &= TCL_LINK_READ_ONLY;
	 return Tcl_LinkVar(interp, temp.get(), reinterpret_cast<char *>(addr),
							  flag | TCL_LINK_INT);
  }
  inline int Tcl_LinkDouble(Tcl_Interp *interp, const char* varName, 
									 double* addr, int flag) {
	 auto_ptr<char> temp(new char[strlen(varName) + 1]);
	 strcpy(temp.get(), varName);
	 flag &= TCL_LINK_READ_ONLY;
	 return Tcl_LinkVar(interp, temp.get(), reinterpret_cast<char *>(addr),
							  flag | TCL_LINK_DOUBLE);
  }
  inline int Tcl_LinkBoolean(Tcl_Interp *interp, const char* varName, 
									  int* addr, int flag) {
	 auto_ptr<char> temp(new char[strlen(varName) + 1]);
	 strcpy(temp.get(), varName);
	 flag &= TCL_LINK_READ_ONLY;
	 return Tcl_LinkVar(interp, temp.get(), reinterpret_cast<char *>(addr),
							  flag | TCL_LINK_BOOLEAN);
  }
  inline int Tcl_LinkString(Tcl_Interp *interp, const char* varName, 
									 char** addr, int flag) {
	 auto_ptr<char> temp(new char[strlen(varName) + 1]);
	 strcpy(temp.get(), varName);
	 flag &= TCL_LINK_READ_ONLY;
	 return Tcl_LinkVar(interp, temp.get(), reinterpret_cast<char *>(addr),
							  flag | TCL_LINK_STRING);
  }  
}

///////////////////////////////////////////////////////////////////////
// Expt Tcl definitions
///////////////////////////////////////////////////////////////////////

Expt& ExptTcl::getExpt() {
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
int ExptTcl::abortTrialCmd(ClientData, Tcl_Interp *interp,
                            int objc, Tcl_Obj *const objv[]) {
DOTRACE("ExptTcl::abortTrialCmd");
  if (objc != 1) {
	 Tcl_WrongNumArgs(interp, 1, objv, NULL);
	 return TCL_ERROR;
  }

  getExpt().abortTrial();
  return TCL_OK;
}

int ExptTcl::beginTrialCmd(ClientData, Tcl_Interp *interp,
                            int objc, Tcl_Obj *const objv[]) {
DOTRACE("ExptTcl::beginTrialCmd");
  if (objc > 2) {
    Tcl_WrongNumArgs(interp, 1, objv, "[verbose]");
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

int ExptTcl::currentStimclassCmd(ClientData, Tcl_Interp *interp,
                                  int objc, Tcl_Obj *const objv[]) {
DOTRACE("ExptTcl::currentStimclassCmd");
  if (objc != 1) {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }

  Tcl_SetObjResult(interp, Tcl_NewIntObj(getExpt().currentStimClass()));
  return TCL_OK;
}

int ExptTcl::currentTrialCmd(ClientData, Tcl_Interp *interp,
                          int objc, Tcl_Obj *const objv[]) {
DOTRACE("ExptTcl::currentTrialCmd");
  if (objc != 1) {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }
  Tcl_SetObjResult(interp, Tcl_NewIntObj(getExpt().currentTrial()));
  return TCL_OK;
}

int ExptTcl::initCmd(ClientData, Tcl_Interp *interp,
                          int objc, Tcl_Obj *const objv[]) {
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

int ExptTcl::isCompleteCmd(ClientData, Tcl_Interp *interp,
                                 int objc, Tcl_Obj *const objv[]) {
DOTRACE("ExptTcl::isCompleteCmd");
  if (objc != 1) {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }
  Tcl_SetObjResult(interp, Tcl_NewIntObj(getExpt().isComplete()));
  return TCL_OK;
}

int ExptTcl::numCompletedCmd(ClientData, Tcl_Interp *interp,
                          int objc, Tcl_Obj *const objv[]) {
DOTRACE("ExptTcl::numCompletedCmd");
  if (objc != 1) {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }
  Tcl_SetObjResult(interp, Tcl_NewIntObj(getExpt().numCompleted()));
  return TCL_OK;
}

int ExptTcl::numTrialsCmd(ClientData, Tcl_Interp *interp,
                          int objc, Tcl_Obj *const objv[]) {
DOTRACE("ExptTcl::numTrialsCmd");
  if (objc != 1) {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }
  Tcl_SetObjResult(interp, Tcl_NewIntObj(getExpt().numTrials()));
  return TCL_OK;
}

int ExptTcl::prevResponseCmd(ClientData, Tcl_Interp *interp,
                          int objc, Tcl_Obj *const objv[]) {
DOTRACE("ExptTcl::prevResponseCmd");
  if (objc != 1) {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }
  Tcl_SetObjResult(interp, Tcl_NewIntObj(getExpt().prevResponse()));
  return TCL_OK;
}

int ExptTcl::readCmd(ClientData, Tcl_Interp *interp,
                          int objc, Tcl_Obj *const objv[]) {
DOTRACE("ExptTcl::readCmd");
  if (objc != 2) {
    Tcl_WrongNumArgs(interp, 1, objv, "filename");
    return TCL_ERROR;
  }
  
  char *filename = Tcl_GetString(objv[1]);
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

int ExptTcl::recordResponseCmd(ClientData, Tcl_Interp *interp,
                                int objc, Tcl_Obj *const objv[]) {
DOTRACE("ExptTcl::recordResponseCmd");
  if (objc != 2 && objc != 3) {
    Tcl_WrongNumArgs(interp, 1, objv, "response [time_in_msec]");
    return TCL_ERROR;
  }

  int response;
  if (Tcl_GetIntFromObj(interp, objv[1], &response) != TCL_OK) return TCL_ERROR;

  int msec = -1;
  if (objc >= 3) 
	 if (Tcl_GetIntFromObj(interp, objv[2], &msec) != TCL_OK) return TCL_ERROR;
  
  getExpt().recordResponse(response, msec);
  return TCL_OK;
}

int ExptTcl::trialDescriptionCmd(ClientData, Tcl_Interp *interp,
                                  int objc, Tcl_Obj *const objv[]) {
DOTRACE("ExptTcl::trialDescriptionCmd");
  if (objc != 1) {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }

  Tcl_SetObjResult(interp, Tcl_NewStringObj(getExpt().trialDescription(), -1));
  return TCL_OK;
}

int ExptTcl::writeCmd(ClientData, Tcl_Interp *interp,
                          int objc, Tcl_Obj *const objv[]) {
DOTRACE("ExptTcl::writeCmd");
  if (objc != 2) {
    Tcl_WrongNumArgs(interp, 1, objv, "filename");
    return TCL_ERROR;
  }
  
  char *filename = Tcl_GetString(objv[1]);
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

int ExptTcl::Expt_Init(Tcl_Interp *interp) {
  DOTRACE("ExptTcl::Expt_Init");
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
  Tcl_CreateObjCommand(interp, "Expt::write", writeCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);

  Tcl_LinkInt(interp, "Expt::clicks1", &clicks1, 0);
  Tcl_LinkInt(interp, "Expt::clicks2", &clicks2, 0);
  Tcl_LinkInt(interp, "Expt::clicks_per_ms", &clicks_per_ms, TCL_LINK_READ_ONLY);
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
