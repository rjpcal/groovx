///////////////////////////////////////////////////////////////////////
// facetcl.cc
// Rob Peters 
// created: Jan-99
// written: Mon Apr 26 14:32:12 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef FACETCL_CC_DEFINED
#define FACETCL_CC_DEFINED

#include "facetcl.h"

#include <tcl.h>
#include <fstream.h>
#include <strstream.h>
#include <cstring>

#include "errmsg.h"
#include "objlist.h"
#include "objlisttcl.h"
#include "face.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

///////////////////////////////////////////////////////////////////////
// Tcl package for Face's
///////////////////////////////////////////////////////////////////////

namespace FaceTcl {
  Face* getFaceFromArg(Tcl_Interp *interp, Tcl_Obj *const objv[],
                       const ObjList& olist, int argn);

  const int BUF_SIZE = 200;

  Tcl_ObjCmdProc faceCmd;
  Tcl_ObjCmdProc paramCmd;		  // single cmd for set/get any parameter
  Tcl_ObjCmdProc loadFacesCmd;
  Tcl_ObjCmdProc stringifyCmd;

  // these constants are passed to err_message() in case of error
  using ObjlistTcl::wrong_type_msg;
}

Face* FaceTcl::getFaceFromArg(Tcl_Interp *interp, Tcl_Obj *const objv[],
                              const ObjList& olist, int argn) {
DOTRACE("FaceTcl::getFaceFromArg");
  // Get the object
  GrObj *g = ObjlistTcl::getObjFromArg(interp, objv, olist, argn);
  if ( g == NULL ) return NULL;
  
  // Make sure it's a face object
  Face *p = dynamic_cast<Face *>(g);
  if ( p == NULL ) {
    err_message(interp, objv, wrong_type_msg);
  }

  return p;
}

int FaceTcl::faceCmd(ClientData, Tcl_Interp *interp,
                     int objc, Tcl_Obj *const objv[]) {
DOTRACE("FaceTcl::faceCmd");
  if (objc > 1) {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }
  
  float eh=0.6, es=0.4, nl=0.4, mh=-0.8;
  Face *p = new Face(eh, es, nl, mh);
  
  ObjList& olist = ObjlistTcl::getObjList();
  int id = olist.addObj(p);

  Tcl_SetObjResult(interp, Tcl_NewIntObj(id));
  return TCL_OK;
}

int FaceTcl::paramCmd(ClientData, Tcl_Interp *interp,
							 int objc, Tcl_Obj *const objv[]) {
DOTRACE("FaceTcl::paramCmd");
  if (objc != 2 && objc != 3) {
    Tcl_WrongNumArgs(interp, 1, objv, "faceid [new_val]");
    return TCL_ERROR;
  }
  
  // Get the param that we will either retrieve or set, and remove a
  // possible leading 'Face::'
  const char* param = Tcl_GetString(objv[0]);
  if ( strncmp(param, "Face::", 6) == 0 ) 
	 param += 6;

  // Get the face
  const ObjList& olist = ObjlistTcl::getObjList();
  Face *p = getFaceFromArg(interp, objv, olist, 1);
  if ( p == NULL ) return TCL_ERROR;

  // Retrieve old value
  if (objc == 2) {
	 float val;

	 if      ( strcmp(param, "eyeHgt")   == 0 ) { val = p->getEyeHgt(); }
	 else if ( strcmp(param, "eyeDist")  == 0 ) { val = p->getEyeDist(); }
	 else if ( strcmp(param, "mouthHgt") == 0 ) { val = p->getMouthHgt(); }
	 else if ( strcmp(param, "noseLen")  == 0 ) { val = p->getNoseLen(); }

	 Tcl_SetObjResult(interp, Tcl_NewDoubleObj(val));
	 return TCL_OK;
  }
  
  // Set new value
  if (objc == 3) {
	 double val;
	 if (Tcl_GetDoubleFromObj(interp, objv[2], &val) != TCL_OK) return TCL_ERROR;

	 if      ( strcmp(param, "eyeHgt")   == 0 ) { p->setEyeHgt(val); }
	 else if ( strcmp(param, "eyeDist")  == 0 ) { p->setEyeDist(val); }
	 else if ( strcmp(param, "mouthHgt") == 0 ) { p->setMouthHgt(val); }
	 else if ( strcmp(param, "noseLen")  == 0 ) { p->setNoseLen(val); }
	 return TCL_OK;
  }
  return TCL_OK;
}

class TestErr {};

int FaceTcl::loadFacesCmd(ClientData, Tcl_Interp *interp,
								  int objc, Tcl_Obj *const objv[]) {
DOTRACE("FaceTcl::loadFacesCmd");
  if (objc < 2 || objc > 4) {
    Tcl_WrongNumArgs(interp, 1, objv, "face_file [num_to_read] [first_id]");
    return TCL_ERROR;
  }

  const char* file = Tcl_GetString(objv[1]);

  int num_to_read = -1;			  // -1 indicates to read to eof
  if (objc >= 3) {
    if (Tcl_GetIntFromObj(interp, objv[2], &num_to_read) != TCL_OK) 
      return TCL_ERROR;
  }

  int first_id = 0;
  if (objc >= 4) {
    if (Tcl_GetIntFromObj(interp, objv[3], &first_id) != TCL_OK) 
      return TCL_ERROR;
  }

  ifstream ifs(file);
  
  ObjList& olist = ObjlistTcl::getObjList();
  char line[BUF_SIZE];

  int objid = first_id;
  int num_read = 0;
  try {
	 while ( (num_to_read < 0 || num_read < num_to_read)
				&& ifs.getline(line, BUF_SIZE)) {
		// allow for whole-line comments beginning with '#'
		if (line[0] == '#') 
		  continue;
		istrstream ist(line);
		Face *p = new Face(ist, IO::NO_FLAGS);
#ifdef LOCAL_DEBUG
		cerr << "after constructor\n";
#endif
		olist.addObjAt(objid, p);
		objid++;
		num_read++;
	 }
  }
  catch (IoError& err) {
#ifdef LOCAL_DEBUG
	 cerr << "in catch block\n";
#endif
	 err_message(interp, objv, err.info().c_str());
	 return TCL_ERROR;
  }

#ifdef LOCAL_DEBUG
		cerr << "after catch blocks\n";
#endif

  Assert(num_read == objid-first_id); // Logic check

  // Return the number of faces created
  Tcl_SetObjResult(interp, Tcl_NewIntObj(num_read));
  return TCL_OK;
}

int FaceTcl::stringifyCmd(ClientData, Tcl_Interp *interp,
								  int objc, Tcl_Obj *const objv[]) {
DOTRACE("FaceTcl::stringifyCmd");  
  if (objc != 2) {
    Tcl_WrongNumArgs(interp, 1, objv, "faceid");
    return TCL_ERROR;
  }
  
  // Get the face
  const ObjList& olist = ObjlistTcl::getObjList();
  const Face *p = getFaceFromArg(interp, objv, olist, 1);
  if ( p == NULL ) return TCL_ERROR;

  char buf[BUF_SIZE];
  ostrstream ost(buf, BUF_SIZE);
  try{
	 p->serialize(ost, IO::TYPENAME|IO::BASES);
	 ost << '\0';
  }
  catch (IoError& err) {
	 err_message(interp, objv, err.info().c_str());
	 return TCL_ERROR;
  }

  Tcl_SetObjResult(interp, Tcl_NewStringObj(buf, -1));
  return TCL_OK;
}

int FaceTcl::Face_Init(Tcl_Interp *interp) {
DOTRACE("FaceTcl::Face_Init");
  // All commands are added to the ::Face namespace
  Tcl_CreateObjCommand(interp, "Face::face", faceCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "Face::mouthHgt", paramCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "Face::noseLen", paramCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "Face::eyeDist", paramCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "Face::eyeHgt", paramCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "Face::loadFaces", loadFacesCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "Face::stringify", stringifyCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);

  Tcl_Eval(interp, 
			  "namespace eval Face {\n"
			  "  namespace export face\n"
			  "}");
  Tcl_Eval(interp,
			  "namespace import Face::face\n");

  // Old (deprecated) commands
  Tcl_Eval(interp,
			  "proc set_nose {faceid nose_length} {\n"
			  "  Face::noseLen $faceid $nose_length}\n"
			  "proc set_mouth {faceid mouth_height} {\n"
			  "  Face::mouthHgt $faceid $mouth_height}\n"
			  "proc set_eyeheight {faceid eye_height} {\n"
			  "  Face::eyeHgt $faceid $eye_height}\n"
			  "proc set_eyedist {faceid distance} {\n"
			  "  Face::eyeDist $faceid $distance}\n");

  Tcl_PkgProvide(interp, "Face", "2.2");
  return TCL_OK;
}

static const char vcid_facetcl_cc[] = "$Header$";
#endif // !FACETCL_CC_DEFINED
