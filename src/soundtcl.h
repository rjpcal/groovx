///////////////////////////////////////////////////////////////////////
//
// soundtcl.h
// Rob Peters
// created: Tue Apr 13 14:09:47 1999
// written: Fri Jul  9 19:28:48 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef SOUNDTCL_H_DEFINED
#define SOUNDTCL_H_DEFINED

struct Tcl_Interp;
typedef int (Tcl_PackageInitProc) (Tcl_Interp* interp);

extern "C" Tcl_PackageInitProc Sound_Init;

static const char vcid_soundtcl_h[] = "$Header$";
#endif // !SOUNDTCL_H_DEFINED
