///////////////////////////////////////////////////////////////////////
//
// soundtcl.h
// Rob Peters
// created: Tue Apr 13 14:09:47 1999
// written: Thu May 27 20:13:42 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef SOUNDTCL_H_DEFINED
#define SOUNDTCL_H_DEFINED

#ifndef _TCL
#  include <tcl.h>
#endif

///////////////////////////////////////////////////////////////////////
//
// SoundTcl class declaration
//
///////////////////////////////////////////////////////////////////////

class SoundTcl {
public:
  SoundTcl(Tcl_Interp* interp);
  ~SoundTcl() ;

  static int playProc(Tcl_Interp* interp, Tcl_Obj* const objv[], 
					const char* snd_name);

  int status() const { return itsStatus; }
private:
  static Tcl_ObjCmdProc setCmd;
  static Tcl_ObjCmdProc playCmd;
  static Tcl_ObjCmdProc haveAudioCmd;

  int itsStatus;
};

static const char vcid_soundtcl_h[] = "$Header$";
#endif // !SOUNDTCL_H_DEFINED
