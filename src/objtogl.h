///////////////////////////////////////////////////////////////////////
// objtogl.h
// Rob Peters
// created: Nov-98
// written: Tue Mar  9 18:39:32 1999
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
