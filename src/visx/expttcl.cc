///////////////////////////////////////////////////////////////////////
// expttcl.cc
// Rob Peters
// created: Mon Mar  8 03:18:40 1999
// written: Tue Mar 16 19:45:54 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef EXPTTCL_CC_DEFINED
#define EXPTTCL_CC_DEFINED

#include "expttcl.h"

#include <fstream.h>
#include <tcl.h>

#include "objlisttcl.h"
#include "tlisttcl.h"
#include "expt.h"
#include "trialexpt.h"
#include "errmsg.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

namespace ExptTcl {
  Expt* theExpt;
  Expt* getExpt() { return theExpt; }
  Expt** getExptHandle() { return &theExpt; }

  Tcl_ObjCmdProc begin_trialCmd;
  Tcl_ObjCmdProc current_stimclassCmd;
  Tcl_ObjCmdProc expt_is_completeCmd;
  Tcl_ObjCmdProc init_exptCmd; 
  Tcl_ObjCmdProc read_exptCmd; 
  Tcl_ObjCmdProc record_responseCmd;
  Tcl_ObjCmdProc trial_descriptionCmd;
  Tcl_ObjCmdProc write_exptCmd;

  // error messages
  const char* const no_expt_msg = ": no expt exists";
}

inline Expt* ExptTcl::getExpt(Tcl_Interp *interp, Tcl_Obj *const objv[]) {
  if ( getExpt() == NULL ) {
    err_message(interp, objv, no_expt_msg);
  }
  return getExpt();
}

int ExptTcl::begin_trialCmd(ClientData, Tcl_Interp *interp,
                            int objc, Tcl_Obj *const objv[]) {
DOTRACE("ExptTcl::begin_trialCmd");
  if (objc > 2) {
    Tcl_WrongNumArgs(interp, 1, objv, "[verbose]");
    return TCL_ERROR;
  }

  int verbose = 0;
  if (objc >= 2) {
    if (Tcl_GetIntFromObj(interp, objv[1], &verbose) != TCL_OK) return TCL_ERROR;
  }

  Expt *expt = getExpt();
  if (expt == NULL) return TCL_ERROR;
  expt->beginTrial();

  if (verbose) {
    Tcl_SetObjResult(interp, Tcl_NewStringObj(expt->trialDescription(), -1));
  }
  return TCL_OK;
}

int ExptTcl::current_stimclassCmd(ClientData, Tcl_Interp *interp,
                                  int objc, Tcl_Obj *const objv[]) {
DOTRACE("ExptTcl::current_stimclassCmd");
  if (objc != 1) {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }

  Expt *expt = getExpt();
  if (expt == NULL) return TCL_ERROR;
  Tcl_SetObjResult(interp, Tcl_NewIntObj(expt->currentStimClass()));
  return TCL_OK;
}

int ExptTcl::expt_is_completeCmd(ClientData, Tcl_Interp *interp,
                                 int objc, Tcl_Obj *const objv[]) {
DOTRACE("ExptTcl::expt_is_completeCmd");
  if (objc != 1) {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }
   
  Expt *expt = getExpt();
  if (expt == NULL) return TCL_ERROR;
  Tcl_SetObjResult(interp, Tcl_NewIntObj(expt->isComplete()));
  return TCL_OK;
}

int ExptTcl::init_exptCmd(ClientData, Tcl_Interp *interp,
                          int objc, Tcl_Obj *const objv[]) {
DOTRACE("ExptTcl::init_exptCmd");
  if (objc != 3) {
    Tcl_WrongNumArgs(interp, 1, objv, "repeat rand_seed");
    return TCL_ERROR;
  }

  int repeat;
  if (Tcl_GetIntFromObj(interp, objv[1], &repeat) != TCL_OK) return TCL_ERROR;

  int rand_seed;
  if (Tcl_GetIntFromObj(interp, objv[2], &rand_seed) != TCL_OK) return TCL_ERROR;

  Expt **expt_h = getExptHandle();
  delete *expt_h;

  *expt_h = new TrialExpt(TlistTcl::getTlist(), repeat, rand_seed);
  if (*expt_h == NULL) return TCL_ERROR;
  return TCL_OK;
}

int ExptTcl::record_responseCmd(ClientData, Tcl_Interp *interp,
                                int objc, Tcl_Obj *const objv[]) {
DOTRACE("ExptTcl::record_responseCmd");
  if (objc != 3) {
    Tcl_WrongNumArgs(interp, 1, objv, "response status");
    return TCL_ERROR;
  }

  int status;
  if (Tcl_GetIntFromObj(interp, objv[2], &status) != TCL_OK) return TCL_ERROR;
  
  // if status is bad, don't record response, but return OK
  if (status <= 0) return TCL_OK;

  int response;
  if (Tcl_GetIntFromObj(interp, objv[1], &response) != TCL_OK) return TCL_ERROR;

  Expt *expt = getExpt();
  if (expt == NULL) return TCL_ERROR;
  expt->recordResponse(response);

  return TCL_OK;
}

int ExptTcl::trial_descriptionCmd(ClientData, Tcl_Interp *interp,
                                  int objc, Tcl_Obj *const objv[]) {
DOTRACE("ExptTcl::trial_descriptionCmd");
  if (objc != 1) {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }

  Expt *expt = getExpt();
  if (expt == NULL) return TCL_ERROR;
  Tcl_SetObjResult(interp, Tcl_NewStringObj(expt->trialDescription(), -1));

  return TCL_OK;
}

int ExptTcl::read_exptCmd(ClientData, Tcl_Interp *interp,
                          int objc, Tcl_Obj *const objv[]) {
DOTRACE("ExptTcl::read_exptCmd");
  if (objc != 2) {
    Tcl_WrongNumArgs(interp, 1, objv, "filename");
    return TCL_ERROR;
  }
  
  char *filename = Tcl_GetString(objv[1]);
  if (filename == NULL) return TCL_ERROR;

  Expt *expt = getExpt();
  if (expt == NULL) return TCL_ERROR;

  ifstream ifs(filename);
  expt->deserialize(ifs, IO::IOFlag(IO::BASES | IO::TYPENAME));
  return TCL_OK;
}

int ExptTcl::write_exptCmd(ClientData, Tcl_Interp *interp,
                          int objc, Tcl_Obj *const objv[]) {
DOTRACE("ExptTcl::write_exptCmd");
  if (objc != 2) {
    Tcl_WrongNumArgs(interp, 1, objv, "filename");
    return TCL_ERROR;
  }
  
  char *filename = Tcl_GetString(objv[1]);
  if (filename == NULL) return TCL_ERROR;

  Expt *expt = getExpt();
  if (expt == NULL) return TCL_ERROR;

  ofstream ofs(filename);
  expt->serialize(ofs, IO::IOFlag(IO::BASES | IO::TYPENAME));
  return TCL_OK;
}

int ExptTcl::Expt_Init(Tcl_Interp *interp) {
DOTRACE("ExptTcl::Expt_Init");
  ExptTcl::theExpt = new TrialExpt(TlistTcl::getTlist(), 0);

  Tcl_CreateObjCommand(interp, "begin_trial", begin_trialCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "current_stimclass", current_stimclassCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "expt_is_complete", expt_is_completeCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "init_expt", init_exptCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "read_expt", read_exptCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "record_response", record_responseCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "trial_description", trial_descriptionCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "write_expt", write_exptCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_PkgProvide(interp, "Expt", "2.1");
  return TCL_OK;
}

static const char vcid_expttcl_cc[] = "$Header$";
#endif // !EXPTTCL_CC_DEFINED
