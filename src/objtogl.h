///////////////////////////////////////////////////////////////////////
// objtogl.h
// Rob Peters
// created: Nov-98
// written: Fri Mar 12 12:55:35 1999
static const char vcid_objtogl_h[] = "$Id$";
//
// This package provides functionality that allows a Togl widget to
// work with a Glist, controlling its display, reshaping, etc.
///////////////////////////////////////////////////////////////////////

#ifndef OBJTOGL_H_DEFINED
#define OBJTOGL_H_DEFINED

struct Tcl_Interp;
typedef int (Tcl_PackageInitProc) (Tcl_Interp *interp);

namespace ObjTogl {
  Tcl_PackageInitProc Objtogl_Init;
}

#endif // !OBJTOGL_H_DEFINED
