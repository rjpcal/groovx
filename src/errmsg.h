///////////////////////////////////////////////////////////////////////
// errmsg.h
// Rob Peters
// created: Dec-98
// written: Wed May 12 14:02:47 1999
// $Id$
//
// This file provides several simple inlined functions that append a
// Tcl function name and a diagnostic error message to the
// interpreters result. There are several ways that the function name
// can be specified: either the entire objv[] or argv[] array from a
// Tcl_CmdProc can be passed to err_message, in which case the value
// of objv[0] or argv[0] is taken to be the function name;
// alternatively a string literal can be passed in as the function
// name.
//
///////////////////////////////////////////////////////////////////////

namespace {

  inline void err_message(Tcl_Interp* interp, Tcl_Obj* const objv[],
                          const char* err_msg) {

	 Tcl_AppendObjToObj(Tcl_GetObjResult(interp), objv[0]);
    Tcl_AppendStringsToObj(Tcl_GetObjResult(interp), 
									": ", err_msg, (char *) NULL); 
  }

  inline void err_message(Tcl_Interp* interp, char* argv[],
                          const char* err_msg) {

    Tcl_AppendStringsToObj(Tcl_GetObjResult(interp), argv[0], 
									": ", err_msg, (char *) NULL); 
  }

  inline void err_message(Tcl_Interp* interp, const char* arg,
                          const char* err_msg) {

    Tcl_AppendStringsToObj(Tcl_GetObjResult(interp), arg, 
									": ", err_msg, (char *) NULL); 
  }

}

static const char vcid_errmsg_h[] = "$Id$";
