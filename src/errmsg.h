///////////////////////////////////////////////////////////////////////
// errmsg.h
// Rob Peters
// created: Dec-98
// written: Fri Mar 12 08:24:08 1999
static const char vcid[] = "$Id$";
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
#if (TCL_MAJOR_VERSION > 8) || \
	 ( (TCL_MAJOR_VERSION == 8) && (TCL_MINOR_VERSION >= 1) )
		Tcl_AppendObjToObj(Tcl_GetObjResult(interp), objv[0]);
	 Tcl_AppendStringsToObj(Tcl_GetObjResult(interp),
									err_msg, (char *) NULL); 
#else
	 Tcl_AppendStringsToObj(Tcl_GetObjResult(interp), 
									Tcl_GetStringFromObj(objv[0], (int *) NULL), 
									err_msg, (char *) NULL); 
#endif
  }

}
