///////////////////////////////////////////////////////////////////////
// soundtcl.h
// Rob Peters
// created: Tue Apr 13 14:09:47 1999
// written: Tue Apr 13 14:10:27 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef SOUNDTCL_H_DEFINED
#define SOUNDTCL_H_DEFINED

struct Tcl_Interp;
typedef int (Tcl_PackageInitProc) (Tcl_Interp *interp);

namespace SoundTcl {
  Tcl_PackageInitProc Sound_Init;
}

static const char vcid_soundtcl_h[] = "$Header$";
#endif // !SOUNDTCL_H_DEFINED
