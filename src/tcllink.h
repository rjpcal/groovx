///////////////////////////////////////////////////////////////////////
//
// tcllink.h
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue May  4 19:16:47 1999
// written: Fri Nov 10 17:27:02 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLLINK_H_DEFINED
#define TCLLINK_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(ARRAYS_H_DEFINED)
#include "util/strings.h"
#endif

#if !defined(TCL_H_DEFINED) && !defined(_TCL)
#include <tcl.h>
#endif

///////////////////////////////////////////////////////////////////////
//
// Helper functions that provide typesafe access to Tcl_LinkVar
//
///////////////////////////////////////////////////////////////////////

namespace Tcl {

  inline int Tcl_LinkInt(Tcl_Interp* interp, const char* varName, 
                         int* addr, int flag) {
	 fixed_string temp = varName;
    flag &= TCL_LINK_READ_ONLY;
    return Tcl_LinkVar(interp, temp.data(), reinterpret_cast<char *>(addr),
                       flag | TCL_LINK_INT);
  }
  inline int Tcl_LinkDouble(Tcl_Interp* interp, const char* varName, 
                            double* addr, int flag) {
	 fixed_string temp = varName;
    flag &= TCL_LINK_READ_ONLY;
    return Tcl_LinkVar(interp, temp.data(), reinterpret_cast<char *>(addr),
                       flag | TCL_LINK_DOUBLE);
  }
  inline int Tcl_LinkBoolean(Tcl_Interp* interp, const char* varName, 
                             int* addr, int flag) {
	 fixed_string temp = varName;
    flag &= TCL_LINK_READ_ONLY;
    return Tcl_LinkVar(interp, temp.data(), reinterpret_cast<char *>(addr),
                       flag | TCL_LINK_BOOLEAN);
  }
  inline int Tcl_LinkString(Tcl_Interp* interp, const char* varName, 
                            char** addr, int flag) {
	 fixed_string temp = varName;
    flag &= TCL_LINK_READ_ONLY;
    return Tcl_LinkVar(interp, temp.data(), reinterpret_cast<char *>(addr),
                       flag | TCL_LINK_STRING);
  }  
}

static const char vcid_tcllink_h[] = "$Header$";
#endif // !TCLLINK_H_DEFINED
