///////////////////////////////////////////////////////////////////////
// glisttcl.cc
// Rob Peters
// created: Jan-99
// written: Fri Mar 12 11:31:40 1999
static const char vcid[] = "$Id$";
///////////////////////////////////////////////////////////////////////

#ifndef GLISTTCL_CC_DEFINED
#define GLISTTCL_CC_DEFINED

#include "glisttcl.h"

#include <GL/gl.h>
#include <tcl.h>

#include "glist.h"
#include "errmsg.h"
#include "objlisttcl.h"

#define NO_TRACE
#include "trace.h"
#include "debug.h"

///////////////////////////////////////////////////////////////////////
// Glist Tcl package
///////////////////////////////////////////////////////////////////////

namespace GlistTcl {
  // For Tcl's purposes, only one Glist (theGlist) is allowed to exist
  // at a time. This Glist* is always passed by its address, so that
  // the Tcl_CmdProc's can have a pointer to this Glist* in their
  // ClientData that is always valid, regardless of whether the actual
  // Glist is allocated and deallocated several times (therefore
  // changing the value of theGlist, but not its address).
  Glist *theGlist = NULL;

  // helper function to retrieve Glist*
  Glist* getGlist(Tcl_Interp *interp, Tcl_Obj *const objv[]);

  Tcl_ObjCmdProc redrawCmd;
  Tcl_ObjCmdProc clearscreenCmd;
  Tcl_ObjCmdProc showCmd;
  Tcl_ObjCmdProc glistSetVisibleCmd;
  Tcl_ObjCmdProc glistAddObjectCmd;
  Tcl_ObjCmdProc glistAddObjectsCmd;
  Tcl_ObjCmdProc glistSetCurGroupCmd;
  Tcl_ObjCmdProc glistExistsCmd;
  Tcl_ObjCmdProc glistDestroyCmd;
  Tcl_ObjCmdProc glistInitCmd;

  Tcl_PackageInitProc Glist_Init;

  // error messages
  using ObjlistTcl::bad_objid_msg;
  const char* const no_glist_msg = ": no glist exists";
}

inline Glist* GlistTcl::getGlist(Tcl_Interp *interp, Tcl_Obj *const objv[]) {
  if ( (*getGlistHandle()) == NULL ) {
	 err_message(interp, objv, no_glist_msg);
  }
  return *getGlistHandle();
}


// this function redraws the Glist's current group to the OpenGL
// window. If the Glist's visibility is zero, nothing will be displayed.
int GlistTcl::redrawCmd(ClientData, Tcl_Interp *interp,
                        int objc, Tcl_Obj *const objv[]) {
DOTRACE("GlistTcl::redrawCmd");
  if (objc > 1) {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }

  Glist *glist = getGlist(interp, objv);
  if (glist == NULL) return TCL_ERROR;
  
  glClear(GL_COLOR_BUFFER_BIT);
  glist->drawCurGroup();
  glFlush();
  return TCL_OK;
}

// this function sets the Glist's visiblity to zero and clears the
// OpenGL window
int GlistTcl::clearscreenCmd(ClientData, Tcl_Interp *interp,
                             int objc, Tcl_Obj *const objv[]) {
DOTRACE("GlistTcl::clearscreenCmd");
  if (objc > 1) {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }

  Glist *glist = getGlist(interp, objv);
  if (glist == NULL) return TCL_ERROR;
  
  glist->setVisible(0);
  glClear(GL_COLOR_BUFFER_BIT);
  glFlush();
  return TCL_OK;
}

// this command draws a specified group in the Glist to the OpenGL
// window. The Glist's current group and visibility are changed accordingly.
int GlistTcl::showCmd(ClientData, Tcl_Interp *interp,
                      int objc, Tcl_Obj *const objv[]) {
DOTRACE("GlistTcl::showCmd");
  if (objc != 2) {
    Tcl_WrongNumArgs(interp, 1, objv, "group_id");
    return TCL_ERROR;
  }
  
  Glist *glist = getGlist(interp, objv);
  if (glist == NULL) return TCL_ERROR;
  
  int id;
  if (Tcl_GetIntFromObj(interp, objv[1], &id) != TCL_OK) return TCL_ERROR;  

  glist->setCurGroup(id);
  glist->setVisible(1);
  glClear(GL_COLOR_BUFFER_BIT);
  glist->drawCurGroup();
  glFlush();
  return TCL_OK;
}

// this function controls the global visibility of a Glist; this
// determines whether anything will be displayed by a "redraw" command
int GlistTcl::glistSetVisibleCmd(ClientData, Tcl_Interp *interp,
                                 int objc, Tcl_Obj *const objv[]) {
DOTRACE("GlistTcl::glistSetVisibleCmd");
  if (objc != 2) {
    Tcl_WrongNumArgs(interp, 1, objv, "visibility"); 
    return TCL_ERROR;
  }

  Glist *glist = getGlist(interp, objv);
  if (glist == NULL) return TCL_ERROR;
  
  int visibility;
  if (Tcl_GetIntFromObj(interp, objv[1], &visibility) != TCL_OK)
    return TCL_ERROR;

  glist->setVisible(visibility);
  return TCL_OK;
}

// this function adds an object (specified by its id# in an ObjList)
// to a specified group in a Glist
int GlistTcl::glistAddObjectCmd(ClientData, Tcl_Interp *interp,
                                int objc, Tcl_Obj *const objv[]) {
DOTRACE("GlistTcl::glistAddObjectCmd");
  if (objc != 3) {
    Tcl_WrongNumArgs(interp, 1, objv, "objid group");
    return TCL_ERROR;
  }

  Glist *glist = getGlist(interp, objv);
  if (glist == NULL) return TCL_ERROR;
  
  int objid, group;
  if (Tcl_GetIntFromObj(interp, objv[1], &objid) != TCL_OK) return TCL_ERROR;
  if (Tcl_GetIntFromObj(interp, objv[2], &group) != TCL_OK) return TCL_ERROR;

  glist->addToGroup(objid, group);
  return TCL_OK;
}

// This function adds to a specified group in the Glist several object
// id's stored together in a Tcl list
int GlistTcl::glistAddObjectsCmd(ClientData, Tcl_Interp *interp,
                                 int objc, Tcl_Obj *const objv[]) {
DOTRACE("GlistTcl::glistAddObjectsCmd");
  if (objc != 3) {
    Tcl_WrongNumArgs(interp, 1, objv, "objid_list group");
    return TCL_ERROR;
  }

  Glist *glist = getGlist(interp, objv);
  if (glist == NULL) return TCL_ERROR;
  
  int src_len, group;
  if (Tcl_ListObjLength(interp, objv[1], &src_len) != TCL_OK) return TCL_ERROR;
  if (Tcl_GetIntFromObj(interp, objv[2], &group) != TCL_OK) return TCL_ERROR;

#ifdef LOCAL_DEBUG
  DUMP_VAL1(src_len);
  DUMP_VAL2(group);
  cerr << "\tadding ";
#endif

  Tcl_Obj *src_elem;
  int objid;
  for (int i = 0; i < src_len; i++) {
    if (Tcl_ListObjIndex(interp, objv[1], i, &src_elem) != TCL_OK)
      return TCL_ERROR;
    if (Tcl_GetIntFromObj(interp, src_elem, &objid) != TCL_OK)
      return TCL_ERROR;
    glist->addToGroup(objid, group);
#ifdef LOCAL_DEBUG
	 DUMP_VAL1(objid);
#endif
  }
  return TCL_OK;
}

// this function sets a Glist's current group; this is the group that
// will be displayed by a subsequent call to "redraw"
int GlistTcl::glistSetCurGroupCmd(ClientData, Tcl_Interp *interp,
                                  int objc, Tcl_Obj *const objv[]) {
DOTRACE("GlistTcl::glistSetCurGroupCmd");
  if (objc != 2) {
    Tcl_WrongNumArgs(interp, 1, objv, "group");
    return TCL_ERROR;
  }

  Glist *glist = getGlist(interp, objv);
  if (glist == NULL) return TCL_ERROR;
  
  int group;
  if (Tcl_GetIntFromObj(interp, objv[1], &group) != TCL_OK) return TCL_ERROR;

  glist->setCurGroup(group);
  return TCL_OK;
}

int GlistTcl::glistExistsCmd(ClientData, Tcl_Interp *interp,
									  int objc, Tcl_Obj *const objv[]) {
DOTRACE("GlistTcl::glistExistsCmd");
  if (objc != 1) {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }

  Glist *glist = *getGlistHandle();
  if (glist == NULL) {
	 Tcl_SetObjResult(interp, Tcl_NewIntObj(0));
  }
  else {
	 Tcl_SetObjResult(interp, Tcl_NewIntObj(1));
  }

  return TCL_OK;
}

// this function destroys a Glist; it must be called before a new
// Glist can be created. Note that destroying a Glist does not destroy
// its associated ObjList, nor any of the objects stored in that ObjList.
int GlistTcl::glistDestroyCmd(ClientData, Tcl_Interp *interp,
                              int objc, Tcl_Obj *const objv[]) {
DOTRACE("GlistTcl::glistDestroyCmd");
  if (objc != 1) {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }

  Glist **glist_h = getGlistHandle();
  if (*glist_h == NULL) {
	 err_message(interp, objv, no_glist_msg);
	 return TCL_ERROR;
  }
  
  delete *glist_h;
  *glist_h = NULL;

  return TCL_OK;
}

// this function initializes a Glist, allocating the space appropriate
// for the number of groups and the size-of-group requested
int GlistTcl::glistInitCmd(ClientData, Tcl_Interp *interp,
                           int objc, Tcl_Obj *const objv[]) {
DOTRACE("GlistTcl::glistInitCmd");
  if (objc != 2) {
    Tcl_WrongNumArgs(interp, 1, objv, "ngroups");
    return TCL_ERROR;
  }

  Glist **glist_h = getGlistHandle();
  if (*glist_h != NULL) {
    err_message(interp, objv,
                ": glist already exists, must first call glistDestroy");
    return TCL_ERROR;
  }

  int num_groups;
  if (Tcl_GetIntFromObj(interp, objv[1], &num_groups) != TCL_OK)
    return TCL_ERROR;

  *glist_h = new Glist(num_groups, *ObjlistTcl::getObjList());

  return TCL_OK;
}

// this is the packaged initialization procedure for Glist. It creates
// all of the Tcl object commands stored in this file.
int GlistTcl::Glist_Init(Tcl_Interp *interp) {
DOTRACE("GlistTcl::Glist_Init");
  Tcl_CreateObjCommand(interp, "redraw", redrawCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "clearscreen", clearscreenCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "show", showCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  
  Tcl_CreateObjCommand(interp, "glistInit", glistInitCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "glistSetVisible", glistSetVisibleCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "glistAddObjects", glistAddObjectsCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "glistAddObject", glistAddObjectCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "glistSetCurGroup", glistSetCurGroupCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "glistExists", glistExistsCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "glistDestroy", glistDestroyCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  return TCL_OK;
}

#endif // !GLISTTCL_CC_DEFINED
