///////////////////////////////////////////////////////////////////////
//
// subjecttcl.h
// Rob Peters
// created: Dec-98
// written: Sun Jun 20 18:11:16 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef SUBJECTTCL_H_DEFINED
#define SUBJECTTCL_H_DEFINED

struct Tcl_Interp;
typedef int (Tcl_PackageInitProc) (Tcl_Interp* interp);

extern "C" Tcl_PackageInitProc Subject_Init;

static const char vcid_subjecttcl_h[] = "$Header$";
#endif // !SUBJECTTCL_H_DEFINED
