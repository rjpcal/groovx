///////////////////////////////////////////////////////////////////////
//
// objlisttcl.h
// Rob Peters
// created: Jan-99
// written: Mon Jun 21 11:09:44 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef OBJLISTTCL_H_DEFINED
#define OBJLISTTCL_H_DEFINED

namespace ObjlistTcl {
  const char* const bad_objid_msg = "objid out of range";
  const char* const wrong_type_msg = "object not of correct type";
}

struct Tcl_Interp;
typedef int (Tcl_PackageInitProc) (Tcl_Interp* interp);

extern "C" Tcl_PackageInitProc Objlist_Init;

static const char vcid_objlisttcl_h[] = "$Header$";
#endif // !OBJLISTTCL_H_DEFINED
