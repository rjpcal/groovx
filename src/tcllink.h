///////////////////////////////////////////////////////////////////////
//
// tcllink.h
// Rob Peters
// created: Tue May  4 19:16:47 1999
// written: Tue Jul 20 15:08:43 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLLINK_H_DEFINED
#define TCLLINK_H_DEFINED

#if !defined(VECTOR_DEFINED) && !defined(__STD_VECTOR__)
#include <vector>
#define VECTOR_DEFINED
#endif

#if !defined(TCL_H_DEFINED) && !defined(_TCL)
#include <tcl.h>
#endif

///////////////////////////////////////////////////////////////////////
//
// Helper functions that provide typesafe access to Tcl_LinkVar
//
///////////////////////////////////////////////////////////////////////

namespace {

  inline int Tcl_LinkInt(Tcl_Interp* interp, const char* varName, 
                         int* addr, int flag) {
    vector<char> temp(strlen(varName));
    strcpy(&(temp[0]), varName);
    flag &= TCL_LINK_READ_ONLY;
    return Tcl_LinkVar(interp, &(temp[0]), reinterpret_cast<char *>(addr),
                       flag | TCL_LINK_INT);
  }
  inline int Tcl_LinkDouble(Tcl_Interp* interp, const char* varName, 
                            double* addr, int flag) {
    vector<char> temp(strlen(varName));
    strcpy(&(temp[0]), varName);
    flag &= TCL_LINK_READ_ONLY;
    return Tcl_LinkVar(interp, &(temp[0]), reinterpret_cast<char *>(addr),
                       flag | TCL_LINK_DOUBLE);
  }
  inline int Tcl_LinkBoolean(Tcl_Interp* interp, const char* varName, 
                             int* addr, int flag) {
    vector<char> temp(strlen(varName));
    strcpy(&(temp[0]), varName);
    flag &= TCL_LINK_READ_ONLY;
    return Tcl_LinkVar(interp, &(temp[0]), reinterpret_cast<char *>(addr),
                       flag | TCL_LINK_BOOLEAN);
  }
  inline int Tcl_LinkString(Tcl_Interp* interp, const char* varName, 
                            char** addr, int flag) {
    vector<char> temp(strlen(varName));
    strcpy(&(temp[0]), varName);
    flag &= TCL_LINK_READ_ONLY;
    return Tcl_LinkVar(interp, &(temp[0]), reinterpret_cast<char *>(addr),
                       flag | TCL_LINK_STRING);
  }  
}

static const char vcid_tcllink_h[] = "$Header$";
#endif // !TCLLINK_H_DEFINED
