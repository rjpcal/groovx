///////////////////////////////////////////////////////////////////////
// tlisttcl.cc
// Rob Peters
// created: Sat Mar 13 12:38:37 1999
// written: Mon Apr 26 19:20:20 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef TLISTTCL_CC_DEFINED
#define TLISTTCL_CC_DEFINED

#include "tlisttcl.h"

#include <tcl.h>
#include <GL/gl.h>
#include <strstream.h>
#include <fstream.h>
#include <iomanip.h>
#include <vector>

#include "tlist.h"
#include "trial.h"
#include "objlist.h"
#include "poslist.h"
#include "objlisttcl.h"
#include "poslisttcl.h"
#include "errmsg.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

///////////////////////////////////////////////////////////////////////
// Tlist Tcl package
///////////////////////////////////////////////////////////////////////

namespace TlistTcl {
  Tcl_ObjCmdProc redrawCmd;
  Tcl_ObjCmdProc clearscreenCmd;
  Tcl_ObjCmdProc showCmd;

  Tcl_ObjCmdProc addObjectCmd;
  Tcl_ObjCmdProc clearAllTrialsCmd;
  Tcl_ObjCmdProc destringifyCmd;
  Tcl_ObjCmdProc makeSummaryTrialCmd;
  Tcl_ObjCmdProc makeSinglesCmd;
  Tcl_ObjCmdProc makePairsCmd;
  Tcl_ObjCmdProc makeTriadsCmd;
  Tcl_ObjCmdProc numTrialsCmd;
  Tcl_ObjCmdProc loadObjidFileCmd;
  Tcl_ObjCmdProc setCurTrialCmd;
  Tcl_ObjCmdProc setVisibleCmd;
  Tcl_ObjCmdProc stringifyCmd;
  Tcl_ObjCmdProc write_responsesCmd;

  // error messages
  const char* const bad_trial_msg = "invalid trial id";
  using ObjlistTcl::bad_objid_msg;
  using PoslistTcl::bad_posid_msg;
}

Tlist& TlistTcl::getTlist() {
  static Tlist theTlist(ObjlistTcl::getObjList(), PoslistTcl::getPosList());
  return theTlist;
}

// this function redraws the Tlist's current trial to the OpenGL
// window. If the Tlist's visibility is zero, nothing will be displayed.
int TlistTcl::redrawCmd(ClientData, Tcl_Interp *interp,
                        int objc, Tcl_Obj *const objv[]) {
DOTRACE("TlistTcl::redrawCmd");
  if (objc > 1) {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }

  const Tlist& tlist = getTlist();
  
  glClear(GL_COLOR_BUFFER_BIT);
  tlist.drawCurTrial();
  glFlush();
  return TCL_OK;
}

// this function sets the Tlist's visiblity to zero and clears the
// OpenGL window
int TlistTcl::clearscreenCmd(ClientData, Tcl_Interp *interp,
                             int objc, Tcl_Obj *const objv[]) {
DOTRACE("TlistTcl::clearscreenCmd");
  if (objc > 1) {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }

  Tlist& tlist = getTlist();
  
  tlist.setVisible(0);
  glClear(GL_COLOR_BUFFER_BIT);
  glFlush();
  return TCL_OK;
}

// this command draws a specified trial in the Tlist to the OpenGL
// window. The Tlist's current trial and visibility are changed accordingly.
int TlistTcl::showCmd(ClientData, Tcl_Interp *interp,
                      int objc, Tcl_Obj *const objv[]) {
DOTRACE("TlistTcl::showCmd");
  if (objc != 2) {
    Tcl_WrongNumArgs(interp, 1, objv, "trial_id");
    return TCL_ERROR;
  }
  
  Tlist& tlist = getTlist();
  
  int id;
  if (Tcl_GetIntFromObj(interp, objv[1], &id) != TCL_OK) return TCL_ERROR;  

  // drawTrial(id) sets the current Trial to id, sets the Tlist's
  // visiblity to true, draws the Trial, and flushes the graphics to
  // the screen
  tlist.drawTrial(id);
  return TCL_OK;
}

// this function adds an object (specified by its id# in an ObjList)
// to a specified trial in a Tlist
int TlistTcl::addObjectCmd(ClientData, Tcl_Interp *interp,
                                int objc, Tcl_Obj *const objv[]) {
DOTRACE("TlistTcl::addObjectCmd");
  if (objc != 4) {
    Tcl_WrongNumArgs(interp, 1, objv, "trial objid posid");
    return TCL_ERROR;
  }

  Tlist& tlist = getTlist();
  
  int trial;
  if (Tcl_GetIntFromObj(interp, objv[1], &trial) != TCL_OK) return TCL_ERROR;
  if (trial < 0) {
	 err_message(interp, objv, bad_trial_msg);
	 return TCL_ERROR;
  }

  int objid;
  if (Tcl_GetIntFromObj(interp, objv[2], &objid) != TCL_OK) return TCL_ERROR;
  if (!ObjlistTcl::getObjList().isValidObjid(objid)) {
	 err_message(interp, objv, bad_objid_msg);
	 return TCL_ERROR;
  }

  int posid;  
  if (Tcl_GetIntFromObj(interp, objv[3], &posid) != TCL_OK) return TCL_ERROR;
  if (!PoslistTcl::getPosList().isValidPosid(posid)) {
	 err_message(interp, objv, bad_posid_msg);
	 return TCL_ERROR;
  }

  tlist.addToTrial(trial, objid, posid);
  return TCL_OK;
}

int TlistTcl::clearAllTrialsCmd(ClientData, Tcl_Interp *interp,
                                  int objc, Tcl_Obj *const objv[]) {
DOTRACE("TlistTcl::clearAllTrialsCmd");
  if (objc != 1) {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }

  Tlist& tlist = getTlist();
  tlist.clearAllTrials();
  return TCL_OK;
}

int TlistTcl::destringifyCmd(ClientData, Tcl_Interp *interp,
									  int objc, Tcl_Obj *const objv[]) {
DOTRACE("TlistTcl::destringifyCmd");
  if (objc != 2) {
    Tcl_WrongNumArgs(interp, 1, objv, "string");
    return TCL_ERROR;
  }

  Tlist& tlist = getTlist();

  const char* buf = Tcl_GetString(objv[1]);
  if (buf == NULL) return TCL_ERROR;

  istrstream ist(buf);

  try {
	 tlist.deserialize(ist, IO::BASES|IO::TYPENAME);
  }
  catch (IoError& err) {
	 err_message(interp, objv, err.info().c_str());
	 return TCL_ERROR;
  }	 

  return TCL_OK;
}

int TlistTcl::makeSinglesCmd(ClientData, Tcl_Interp *interp,
                                  int objc, Tcl_Obj *const objv[]) {
DOTRACE("TlistTcl::makeSinglesCmd");
  if (objc != 2) {
    Tcl_WrongNumArgs(interp, 1, objv, "posid");
    return TCL_ERROR;
  }

  int posid;
  if (Tcl_GetIntFromObj(interp, objv[1], &posid) != TCL_OK) return TCL_ERROR;
  if (!PoslistTcl::getPosList().isValidPosid(posid)) {
	 err_message(interp, objv, bad_posid_msg);
	 return TCL_ERROR;
  }

  vector<int> vec;
  const ObjList& olist = ObjlistTcl::getObjList();
  olist.getValidObjids(vec);

  Tlist& tlist = getTlist();
  tlist.clearAllTrials();

  // This loop runs through all the trials and does two things: 
  // 1) it adds the appropriate objid and the given posid to the trial
  // 2) it sets the Trial's type to the category of its single GrObj
  int trialid;
  for (trialid = 0; trialid < vec.size(); trialid++) {
    tlist.addToTrial(trialid, vec[trialid], posid);
	 const GrObj* obj = olist.getObj(vec[trialid]);
	 Assert(obj);
	 int categ = obj->getCategory();
	 tlist.getTrial(trialid).setType(categ);
  }

  // Return the number of trials that were generated
  Tcl_SetObjResult(interp, Tcl_NewIntObj(trialid));
  return TCL_OK;
}

int TlistTcl::makeSummaryTrialCmd(ClientData, Tcl_Interp *interp,
                          int objc, Tcl_Obj *const objv[]) {
DOTRACE("TlistTcl::makeSummaryTrialCmd");
  if (objc < 4 || objc > 6) {
    Tcl_WrongNumArgs(interp, 1, objv, "trialid num_cols scale [xstep] [ystep]");
    return TCL_ERROR;
  }

  int trialid;
  if (Tcl_GetIntFromObj(interp, objv[1], &trialid) != TCL_OK)
	 return TCL_ERROR;
  if (trialid < 0) {
	 err_message(interp, objv, bad_trial_msg);
	 return TCL_ERROR;
  }

  int num_cols;
  if (Tcl_GetIntFromObj(interp, objv[2], &num_cols) != TCL_OK)
	 return TCL_ERROR;
  if (num_cols <= 0) {
	 err_message(interp, objv, "num_cols must be a positive integer");
	 return TCL_ERROR;
  }

  double scale;
  if (Tcl_GetDoubleFromObj(interp, objv[3], &scale) != TCL_OK)
	 return TCL_ERROR;

  double xstep = 2.0;
  if (objc >= 5)
	 if (Tcl_GetDoubleFromObj(interp, objv[4], &xstep) != TCL_OK)
		return TCL_ERROR;
	 
  double ystep = 3.0;
  if (objc >= 6)
	 if (Tcl_GetDoubleFromObj(interp, objv[5], &ystep) != TCL_OK)
		return TCL_ERROR;

  ObjList& olist = ObjlistTcl::getObjList();
  int num_objs = olist.objCount();

  // Figure number of columns-- (num_objs-1)/num_cols gives one fewer
  // than the number of rows that we need, so add 1 to it
  int num_rows = 1 + (num_objs-1)/num_cols;

  vector<int> objids;
  olist.getValidObjids(objids);

  // Coords of upper left corner of viewing area
  const float x0 = scale * (0.0 - xstep*(num_cols-1)/2.0);
  const float y0 = scale * (0.0 + ystep*(num_rows-1)/2.0);

#ifdef LOCAL_DEBUG
  DUMP_VAL1(num_objs);
  DUMP_VAL1(num_cols);
  DUMP_VAL2(num_rows);
  DUMP_VAL1(scale);
  DUMP_VAL1(xstep);
  DUMP_VAL1(ystep);
  DUMP_VAL1(x0);
  DUMP_VAL2(y0);
#endif

  PosList& plist = PoslistTcl::getPosList();
  Tlist& tlist = getTlist();

  for (int i=0; i < objids.size(); i++) {
	 int row = i / num_cols;
	 int col = i % num_cols;
	 Position *p = new Position;
	 float xpos = x0+col*xstep*scale;
	 float ypos = y0-row*ystep*scale;
#ifdef LOCAL_DEBUG
	 DUMP_VAL1(xpos);
	 DUMP_VAL2(ypos);
#endif
	 p->setTranslate(xpos, ypos, 0.0);
	 p->setScale(scale, scale, 1.0);
	 int posid = plist.addPos(p);
	 tlist.addToTrial(trialid, objids[i], posid);
  }
  Tcl_SetObjResult(interp, Tcl_NewIntObj(trialid));
  return TCL_OK;
}

int TlistTcl::makePairsCmd(ClientData, Tcl_Interp *interp,
                                int objc, Tcl_Obj *const objv[]) {
DOTRACE("TlistTcl::makePairsCmd");
  if (objc != 3) {
    Tcl_WrongNumArgs(interp, 1, objv, "posid1 posid2");
    return TCL_ERROR;
  }

  int posid1, posid2;
  if (Tcl_GetIntFromObj(interp, objv[1], &posid1) != TCL_OK) return TCL_ERROR;
  if (Tcl_GetIntFromObj(interp, objv[2], &posid2) != TCL_OK) return TCL_ERROR;
  const PosList& plist = PoslistTcl::getPosList();
  if (!plist.isValidPosid(posid1) || !plist.isValidPosid(posid2)) {
	 err_message(interp, objv, bad_posid_msg);
	 return TCL_ERROR;
  }

  vector<int> vec;
  const ObjList& olist = ObjlistTcl::getObjList();
  olist.getValidObjids(vec);

  Tlist& tlist = getTlist();
  tlist.clearAllTrials();

  int trialid = 0;
  for (int i = 0; i < vec.size(); i++) {
    for (int j = 0; j < vec.size(); j++) {
      tlist.addToTrial(trialid, vec[i], posid1);
      tlist.addToTrial(trialid, vec[j], posid2);
      trialid++;
    }
  }

  // Return the number of trials that were generated
  Tcl_SetObjResult(interp, Tcl_NewIntObj(trialid));
  return TCL_OK;
}

int TlistTcl::makeTriadsCmd(ClientData, Tcl_Interp *interp,
                                 int objc, Tcl_Obj *const objv[]) {
DOTRACE("TlistTcl::makeTriadsCmd");
  if (objc != 4) {
    Tcl_WrongNumArgs(interp, 1, objv, "posid1 posid2 posid3");
    return TCL_ERROR;
  }

  int posid[3];
  if (Tcl_GetIntFromObj(interp, objv[1], posid+0) != TCL_OK) return TCL_ERROR;
  if (Tcl_GetIntFromObj(interp, objv[2], posid+1) != TCL_OK) return TCL_ERROR;
  if (Tcl_GetIntFromObj(interp, objv[3], posid+2) != TCL_OK) return TCL_ERROR;
  const PosList& plist = PoslistTcl::getPosList();
  if (!plist.isValidPosid(posid[0]) || !plist.isValidPosid(posid[1]) ||
		!plist.isValidPosid(posid[2])) {
	 err_message(interp, objv, bad_posid_msg);
	 return TCL_ERROR;
  }
#ifdef LOCAL_DEBUG
  DUMP_VAL1(posid[0]);
  DUMP_VAL1(posid[1]);
  DUMP_VAL2(posid[2]);
#endif

  vector<int> vec;
  const ObjList& olist = ObjlistTcl::getObjList();
  olist.getValidObjids(vec);

  Tlist& tlist = getTlist();
  tlist.clearAllTrials();

  const int NUM_PERMS = 18;
  static int permutations[NUM_PERMS][3] = { 
    {0, 0, 1},
    {0, 0, 2},
    {1, 1, 0},
    {1, 1, 2},
    {2, 2, 0},
    {2, 2, 1},
    {0, 1, 1},
    {0, 2, 2},
    {1, 0, 0},
    {2, 0, 0},
    {2, 1, 1},
    {1, 2, 2},
    {0, 1, 2},
    {0, 2, 1},
    {1, 0, 2},
    {1, 2, 0},
    {2, 0, 1},
    {2, 1, 0} };
  
  int num_objs = vec.size();
  int base_triad[3];
    
  int trial = 0;

  // loops over i,j,k construct all possible base triads
  for (int i = 0; i < num_objs-2; i++) {
    base_triad[0] = vec[i];
    for (int j = i+1; j < num_objs; j++) {
      base_triad[1] = vec[j];
      for (int k = j+1; k < num_objs; k++) {
        base_triad[2] = vec[k];
#ifdef LOCAL_DEBUG
        DUMP_VAL1(i);
        DUMP_VAL1(j);
        DUMP_VAL1(k);
        DUMP_VAL2(trial);
#endif

        // loops over p,e run through all permutations
        for (int p = 0; p < NUM_PERMS; p++) {
          for (int e = 0; e < 3; e++) {
            tlist.addToTrial(trial, base_triad[e], posid[e]);
          }
          trial++;
        } // end p

      } // end k
    } // end j
  } // end i

  // Return the number of trials generated
  Tcl_SetObjResult(interp, Tcl_NewIntObj(trial));
  return TCL_OK;
}

int TlistTcl::numTrialsCmd(ClientData, Tcl_Interp *interp,
									int objc, Tcl_Obj *const objv[]) {
DOTRACE("TlistTcl::numTrialsCmd");
  if (objc != 1) {
	 Tcl_WrongNumArgs(interp, 1, objv, NULL);
	 return TCL_ERROR;
  }

  Tcl_SetObjResult(interp, Tcl_NewIntObj(getTlist().trialCount()));
  return TCL_OK;
}

// This procedure initializes the Tlist from objid_file, which must be
// the name of a file containing one list of objid's per
// line. num_lines specifies how many lines to read, or, if set to -1,
// the entire file will be read. offset is an optional parameter
// which, if provided, will be added to each objid before the objid is
// inserted into the Tlist.
int TlistTcl::loadObjidFileCmd(ClientData, Tcl_Interp *interp,
                                  int objc, Tcl_Obj *const objv[]) {
DOTRACE("TlistTcl::loadObjidFileCmd");
  if (objc < 3 || objc > 4) {
    Tcl_WrongNumArgs(interp, 1, objv, "objid_file num_lines [offset]");
    return TCL_ERROR;
  }

  const char* objid_file = Tcl_GetString(objv[1]);
  if (objid_file == NULL) return TCL_ERROR;

  int num_lines = -1;
  if (Tcl_GetIntFromObj(interp, objv[2], &num_lines) != TCL_OK) return TCL_ERROR;

  int offset = 0;
  if (objc >= 4) {
    if (Tcl_GetIntFromObj(interp, objv[3], &offset) != TCL_OK) return TCL_ERROR;
  }

  Tlist& tlist = getTlist();

  ifstream ifs(objid_file);
  try {
	 tlist.readFromObjidsOnly(ifs, num_lines, offset);
  }
  catch (IoError& err) {
	 err_message(interp, objv, err.info().c_str());
	 return TCL_ERROR;
  }
  return TCL_OK;
}

// this function sets a Tlist's current trial; this is the trial that
// will be displayed by a subsequent call to "redraw"
int TlistTcl::setCurTrialCmd(ClientData, Tcl_Interp *interp,
                                  int objc, Tcl_Obj *const objv[]) {
DOTRACE("TlistTcl::setCurTrialCmd");
  if (objc != 2) {
    Tcl_WrongNumArgs(interp, 1, objv, "trial");
    return TCL_ERROR;
  }

  Tlist& tlist = getTlist();
  
  int trial;
  if (Tcl_GetIntFromObj(interp, objv[1], &trial) != TCL_OK) return TCL_ERROR;
  if (!tlist.isValidTrial(trial)) {
	 err_message(interp, objv, bad_trial_msg);
	 return TCL_ERROR;
  }
  
  tlist.setCurTrial(trial);
  return TCL_OK;
}

// this function controls the global visibility of a Tlist; this
// determines whether anything will be displayed by a "redraw" command
int TlistTcl::setVisibleCmd(ClientData, Tcl_Interp *interp,
                                 int objc, Tcl_Obj *const objv[]) {
DOTRACE("TlistTcl::setVisibleCmd");
  if (objc != 2) {
    Tcl_WrongNumArgs(interp, 1, objv, "visibility"); 
    return TCL_ERROR;
  }

  Tlist& tlist = getTlist();
  
  int visibility;
  if (Tcl_GetIntFromObj(interp, objv[1], &visibility) != TCL_OK)
    return TCL_ERROR;

  tlist.setVisible(visibility);
  return TCL_OK;
}

int TlistTcl::stringifyCmd(ClientData, Tcl_Interp *interp,
									int objc, Tcl_Obj *const objv[]) {
DOTRACE("TlistTcl::stringifyCmd");
  if (objc != 1) {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }

  const int BUF_SIZE = 2000;
  char buf[BUF_SIZE];
  ostrstream ost(buf, BUF_SIZE);

  const Tlist& tlist = getTlist();
  try {
	 tlist.serialize(ost, IO::BASES|IO::TYPENAME);
	 ost << '\0';
  }
  catch (IoError& err) {
	 err_message(interp, objv, err.info().c_str());
	 return TCL_ERROR;
  }	 

  Tcl_SetObjResult(interp, Tcl_NewStringObj(buf, -1));
  return TCL_OK;
}

int TlistTcl::write_responsesCmd(ClientData, Tcl_Interp *interp,
											int objc, Tcl_Obj *const objv[]) {
DOTRACE("TlistTcl::write_responsesCmd");
  if (objc != 2) {
    Tcl_WrongNumArgs(interp, 1, objv, "filename");
    return TCL_ERROR;
  }

  char *filename = Tcl_GetString(objv[1]);
  Assert(filename);

  const Tlist& tlist = TlistTcl::getTlist();
  vector<int> trialids;
  tlist.getValidTrials(trialids);
#ifdef LOCAL_DEBUG
  DUMP_VAL2(trialids.size());
#endif

  ofstream ofs(filename);
  const int wid = 8;

  // We prepend a '%' to the header line so that MATLAB can ignore
  // this line as a comment
  ofs << '%' << setw(wid-1) << "Trial" << setw(wid) << "N" 
		<< setw(wid) << "Average" << setw(wid) << "msec\n";
  
  ofs.setf(ios::fixed);
  ofs.precision(2);
  for (int i = 0; i < trialids.size(); i++) {
	 const Trial& t = tlist.getTrial(trialids[i]);
	 ofs << setw(wid) << trialids[i];
	 ofs << setw(wid) << t.numResponses();
	 ofs << setw(wid) << t.avgResponse();	 
	 ofs << setw(wid) << t.avgRespTime() << endl;
  }
  return TCL_OK;
}

int TlistTcl::Tlist_Init(Tcl_Interp *interp) {
DOTRACE("TlistTcl::Tlist_Init");
  // Add all commands to ::Tlist namespace
  Tcl_CreateObjCommand(interp, "Tlist::redraw", redrawCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "Tlist::clearscreen", clearscreenCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "Tlist::show", showCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  
  Tcl_CreateObjCommand(interp, "Tlist::addObject", addObjectCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "Tlist::clearAllTrials", clearAllTrialsCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "Tlist::destringify", destringifyCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "Tlist::makeSummaryTrial", makeSummaryTrialCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "Tlist::makeSingles", makeSinglesCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "Tlist::makePairs", makePairsCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "Tlist::makeTriads", makeTriadsCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "Tlist::numTrials", numTrialsCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "Tlist::loadObjidFile", loadObjidFileCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "Tlist::setCurTrial", setCurTrialCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "Tlist::setVisible", setVisibleCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "Tlist::stringify", stringifyCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "Tlist::write_responses", write_responsesCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);

  Tcl_Eval(interp,
			  "namespace eval Tlist {\n"
			  "  namespace export redraw\n"
			  "  namespace export clearscreen\n"
			  "  namespace export show\n"
			  "}");
  Tcl_Eval(interp,
			  "namespace import Tlist::redraw\n"
			  "namespace import Tlist::clearscreen\n"
			  "namespace import Tlist::show\n");

  Tcl_PkgProvide(interp, "Tlist", "2.1");
  return TCL_OK;
}

static const char vcid_tlisttcl_cc[] = "$Header$";
#endif // !TLISTTCL_CC_DEFINED
