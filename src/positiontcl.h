///////////////////////////////////////////////////////////////////////
//
// positiontcl.h
// Rob Peters
// created: Sat Mar 13 12:52:55 1999
// written: Sun Jun 20 17:38:37 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef POSITIONTCL_H_DEFINED
#define POSITIONTCL_H_DEFINED

struct Tcl_Interp;
typedef int (Tcl_PackageInitProc) (Tcl_Interp* interp);

extern "C" Tcl_PackageInitProc Pos_Init;

static const char vcid_positiontcl_h[] = "$Header$";
#endif // !POSITIONTCL_H_DEFINED
