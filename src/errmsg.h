///////////////////////////////////////////////////////////////////////
// errmsg.h
// Rob Peters
// created: Dec-98
// written: Wed Apr 14 20:22:55 1999
// $Id$
//
// this provides a simple inlined function that appends a Tcl function name
// and an error message to the interpreters result
//
// this is provided as a function so that the Tcl library calls can be
// modified when the API changes
///////////////////////////////////////////////////////////////////////

namespace {

  inline void err_message(Tcl_Interp *interp, Tcl_Obj *const objv[],
                          const char * err_msg) {

	 Tcl_AppendObjToObj(Tcl_GetObjResult(interp), objv[0]);
    Tcl_AppendStringsToObj(Tcl_GetObjResult(interp), 
									": ", err_msg, (char *) NULL); 
  }

  inline void err_message(Tcl_Interp *interp, char *argv[],
                          const char * err_msg) {

    Tcl_AppendStringsToObj(Tcl_GetObjResult(interp), argv[0], 
									": ", err_msg, (char *) NULL); 
  }

}

static const char vcid_errmsg_h[] = "$Id$";
