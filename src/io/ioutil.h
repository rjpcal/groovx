///////////////////////////////////////////////////////////////////////
//
// stringifycmd.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Fri Jun 11 21:43:43 1999
// written: Mon Jun 14 12:18:33 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef STRINGIFYCMD_H_DEFINED
#define STRINGIFYCMD_H_DEFINED

#ifndef TCLCMD_H_DEFINED
#include "tclcmd.h"
#endif

class IO;

class StringifyCmd : public TclCmd {
public:
  StringifyCmd(Tcl_Interp* interp, const char* cmd_name, 
					const char* usage, int objc) :
	 TclCmd(interp, cmd_name, usage, objc, objc, true) {}

protected:
  virtual IO& getIO() = 0;
  virtual int getBufSize() = 0;

private:
  virtual void invoke();
};

// It is assumed that the string is contained in the last argument
class DestringifyCmd : public TclCmd {
public:
  DestringifyCmd(Tcl_Interp* interp, const char* cmd_name, 
					  const char* usage, int objc) :
	 TclCmd(interp, cmd_name, usage, objc, objc, true) {}

protected:
  virtual IO& getIO() = 0;

private:
  virtual void invoke();
};

static const char vcid_stringifycmd_h[] = "$Header$";
#endif // !STRINGIFYCMD_H_DEFINED
