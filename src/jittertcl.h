///////////////////////////////////////////////////////////////////////
//
// jittertcl.h
// Rob Peters
// created: Wed Apr  7 14:58:07 1999
// written: Thu May 27 19:56:33 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef JITTERTCL_H_DEFINED
#define JITTERTCL_H_DEFINED

struct Tcl_Interp;
typedef int (Tcl_PackageInitProc) (Tcl_Interp* interp);

namespace JitterTcl {
  Tcl_PackageInitProc Jitter_Init;
}

static const char vcid_jittertcl_h[] = "$Header$";
#endif // !JITTERTCL_H_DEFINED
