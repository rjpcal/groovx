///////////////////////////////////////////////////////////////////////
//
// jittertcl.h
// Rob Peters
// created: Wed Apr  7 14:58:07 1999
// written: Sun Jun 20 18:00:24 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef JITTERTCL_H_DEFINED
#define JITTERTCL_H_DEFINED

struct Tcl_Interp;
typedef int (Tcl_PackageInitProc) (Tcl_Interp* interp);

extern "C" Tcl_PackageInitProc Jitter_Init;

static const char vcid_jittertcl_h[] = "$Header$";
#endif // !JITTERTCL_H_DEFINED
