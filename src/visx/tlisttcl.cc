///////////////////////////////////////////////////////////////////////
// tlisttcl.cc
// Rob Peters
// created: Sat Mar 13 12:38:37 1999
// written: Tue Mar 16 19:23:19 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef TLISTTCL_CC_DEFINED
#define TLISTTCL_CC_DEFINED

#include "tlisttcl.h"

#include <tcl.h>
#include <GL/gl.h>
#include <strstream.h>
#include <fstream.h>
#include <vector>

#include "tlist.h"
#include "objlist.h"
#include "objlisttcl.h"
#include "poslisttcl.h"
#include "errmsg.h"

#include "trace.h"
#include "debug.h"

///////////////////////////////////////////////////////////////////////
// Tlist Tcl package
///////////////////////////////////////////////////////////////////////

namespace TlistTcl {
  Tcl_ObjCmdProc redrawCmd;
  Tcl_ObjCmdProc clearscreenCmd;
  Tcl_ObjCmdProc showCmd;
  Tcl_ObjCmdProc tlistSetVisibleCmd;
  Tcl_ObjCmdProc tlistAddObjectCmd;
  Tcl_ObjCmdProc tlistSetCurTrialCmd;

  Tcl_ObjCmdProc tlistReadFromObjidFileCmd;

  Tcl_ObjCmdProc tlistMakeSinglesCmd;
  Tcl_ObjCmdProc tlistMakePairsCmd;
  Tcl_ObjCmdProc tlistMakeTriadsCmd;

  Tcl_ObjCmdProc stringify_tlistCmd;
  Tcl_ObjCmdProc destringify_tlistCmd;

  // error messages
  using ObjlistTcl::bad_objid_msg;
  const char* const no_tlist_msg = ": no tlist exists";
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

// this function controls the global visibility of a Tlist; this
// determines whether anything will be displayed by a "redraw" command
int TlistTcl::tlistSetVisibleCmd(ClientData, Tcl_Interp *interp,
                                 int objc, Tcl_Obj *const objv[]) {
DOTRACE("TlistTcl::tlistSetVisibleCmd");
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

// this function adds an object (specified by its id# in an ObjList)
// to a specified trial in a Tlist
int TlistTcl::tlistAddObjectCmd(ClientData, Tcl_Interp *interp,
                                int objc, Tcl_Obj *const objv[]) {
DOTRACE("TlistTcl::tlistAddObjectCmd");
  if (objc != 4) {
    Tcl_WrongNumArgs(interp, 1, objv, "trial objid posid");
    return TCL_ERROR;
  }

  Tlist& tlist = getTlist();
  
  int trial, objid, posid;
  if (Tcl_GetIntFromObj(interp, objv[1], &trial) != TCL_OK) return TCL_ERROR;
  if (Tcl_GetIntFromObj(interp, objv[2], &objid) != TCL_OK) return TCL_ERROR;
  if (Tcl_GetIntFromObj(interp, objv[3], &posid) != TCL_OK) return TCL_ERROR;

  tlist.addToTrial(trial, objid, posid);
  return TCL_OK;
}

// this function sets a Tlist's current trial; this is the trial that
// will be displayed by a subsequent call to "redraw"
int TlistTcl::tlistSetCurTrialCmd(ClientData, Tcl_Interp *interp,
                                  int objc, Tcl_Obj *const objv[]) {
DOTRACE("TlistTcl::tlistSetCurTrialCmd");
  if (objc != 2) {
    Tcl_WrongNumArgs(interp, 1, objv, "trial");
    return TCL_ERROR;
  }

  Tlist& tlist = getTlist();
  
  int trial;
  if (Tcl_GetIntFromObj(interp, objv[1], &trial) != TCL_OK) return TCL_ERROR;

  tlist.setCurTrial(trial);
  return TCL_OK;
}

int TlistTcl::tlistReadFromObjidFileCmd(ClientData, Tcl_Interp *interp,
                                  int objc, Tcl_Obj *const objv[]) {
DOTRACE("TlistTcl::tlistReadFromObjidFileCmd");
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
  if (tlist.readFromObjidsOnly(ifs, num_lines, offset) != IO_OK)
    return TCL_ERROR;
  return TCL_OK;
}

int TlistTcl::tlistMakeSinglesCmd(ClientData, Tcl_Interp *interp,
                                  int objc, Tcl_Obj *const objv[]) {
DOTRACE("TlistTcl::tlistMakeSinglesCmd");
  if (objc != 2) {
    Tcl_WrongNumArgs(interp, 1, objv, "posid");
    return TCL_ERROR;
  }

  int posid;
  if (Tcl_GetIntFromObj(interp, objv[1], &posid) != TCL_OK) return TCL_ERROR;

  vector<int> vec;
  const ObjList& olist = ObjlistTcl::getObjList();
  olist.getValidObjids(vec);

  Tlist& tlist = getTlist();
  tlist.clearAllTrials();

  int trial;
  for (trial = 0; trial < vec.size(); trial++) {
    tlist.addToTrial(trial, vec[trial], posid);
  }

  // Return the number of trials that were generated
  Tcl_SetObjResult(interp, Tcl_NewIntObj(trial));
  return TCL_OK;
}

int TlistTcl::tlistMakePairsCmd(ClientData, Tcl_Interp *interp,
                                int objc, Tcl_Obj *const objv[]) {
DOTRACE("TlistTcl::tlistMakePairsCmd");
  if (objc != 3) {
    Tcl_WrongNumArgs(interp, 1, objv, "posid1 posid2");
    return TCL_ERROR;
  }

  int posid1, posid2;
  if (Tcl_GetIntFromObj(interp, objv[1], &posid1) != TCL_OK) return TCL_ERROR;
  if (Tcl_GetIntFromObj(interp, objv[2], &posid2) != TCL_OK) return TCL_ERROR;

  vector<int> vec;
  const ObjList& olist = ObjlistTcl::getObjList();
  olist.getValidObjids(vec);

  Tlist& tlist = getTlist();
  tlist.clearAllTrials();

  int trial = 0;
  for (int i = 0; i < vec.size(); i++) {
    for (int j = 0; j < vec.size(); j++) {
      tlist.addToTrial(trial, vec[i], posid1);
      tlist.addToTrial(trial, vec[j], posid2);
      trial++;
    }
  }

  // Return the number of trials that were generated
  Tcl_SetObjResult(interp, Tcl_NewIntObj(trial));
  return TCL_OK;
}

int TlistTcl::tlistMakeTriadsCmd(ClientData, Tcl_Interp *interp,
                                 int objc, Tcl_Obj *const objv[]) {
DOTRACE("TlistTcl::tlistMakeTriadsCmd");
  if (objc != 4) {
    Tcl_WrongNumArgs(interp, 1, objv, "posid1 posid2 posid3");
    return TCL_ERROR;
  }

  int posid[3];
  if (Tcl_GetIntFromObj(interp, objv[1], posid+0) != TCL_OK) return TCL_ERROR;
  if (Tcl_GetIntFromObj(interp, objv[2], posid+1) != TCL_OK) return TCL_ERROR;
  if (Tcl_GetIntFromObj(interp, objv[3], posid+2) != TCL_OK) return TCL_ERROR;
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

int TlistTcl::stringify_tlistCmd(ClientData, Tcl_Interp *interp,
                                 int objc, Tcl_Obj *const objv[]) {
DOTRACE("TlistTcl::stringify_tlistCmd");
  if (objc != 1) {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }

  Tlist& tlist = getTlist();

  const int BUF_SIZE = 400;
  char buf[BUF_SIZE];
  ostrstream ost(buf, BUF_SIZE);

  tlist.serialize(ost, IO::IOFlag(IO::BASES|IO::TYPENAME));

  Tcl_SetObjResult(interp, Tcl_NewStringObj(buf, -1));
  return TCL_OK;
}

int TlistTcl::destringify_tlistCmd(ClientData, Tcl_Interp *interp,
                                 int objc, Tcl_Obj *const objv[]) {
DOTRACE("TlistTcl::destringify_tlistCmd");
  if (objc != 2) {
    Tcl_WrongNumArgs(interp, 1, objv, "string");
    return TCL_ERROR;
  }

  Tlist& tlist = getTlist();

  const char* buf = Tcl_GetString(objv[1]);
  if (buf == NULL) return TCL_ERROR;

  istrstream ist(buf);

  tlist.deserialize(ist, IO::IOFlag(IO::BASES|IO::TYPENAME));

  return TCL_OK;
}

int TlistTcl::Tlist_Init(Tcl_Interp *interp) {
DOTRACE("TlistTcl::Tlist_Init");
  Tcl_CreateObjCommand(interp, "redraw", redrawCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "clearscreen", clearscreenCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "show", showCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  
  Tcl_CreateObjCommand(interp, "tlistSetVisible", tlistSetVisibleCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "tlistAddObject", tlistAddObjectCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "tlistSetCurTrial", tlistSetCurTrialCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);

  Tcl_CreateObjCommand(interp, "tlistReadFromObjidFile", 
                       tlistReadFromObjidFileCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);

  Tcl_CreateObjCommand(interp, "tlistMakeSingles", tlistMakeSinglesCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "tlistMakePairs", tlistMakePairsCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "tlistMakeTriads", tlistMakeTriadsCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);

  Tcl_CreateObjCommand(interp, "stringify_tlist", stringify_tlistCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "destringify_tlist", destringify_tlistCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);

  Tcl_PkgProvide(interp, "Tlist", "2.1");
  return TCL_OK;
}

static const char vcid_tlisttcl_cc[] = "$Header$";
#endif // !TLISTTCL_CC_DEFINED
