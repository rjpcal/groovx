///////////////////////////////////////////////////////////////////////
//
// stringifycmd.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Fri Jun 11 21:43:43 1999
// written: Thu Mar 30 08:29:17 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef STRINGIFYCMD_H_DEFINED
#define STRINGIFYCMD_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TCLCMD_H_DEFINED)
#include "tcl/tclcmd.h"
#endif

namespace IO { class IoObject; }

namespace Tcl {

class StringifyCmd : public TclCmd {
public:
  StringifyCmd(Tcl_Interp* interp, const char* cmd_name, 
					const char* usage, int objc) :
	 TclCmd(interp, cmd_name, usage, objc, objc, true) {}

protected:
  virtual IO::IoObject& getIO() = 0;

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
  virtual IO::IoObject& getIO() = 0;

private:
  virtual void invoke();
};

class WriteCmd : public TclCmd {
public:
  WriteCmd(Tcl_Interp* interp, const char* cmd_name,
			  const char* usage, int objc) :
	 TclCmd(interp, cmd_name, usage, objc, objc, true) {}

protected:
  virtual IO::IoObject& getIO() = 0;

private:
  virtual void invoke();
};

class ReadCmd : public TclCmd {
public:
  ReadCmd(Tcl_Interp* interp, const char* cmd_name,
			 const char* usage, int objc) :
	 TclCmd(interp, cmd_name, usage, objc, objc, true) {}

protected:
  virtual IO::IoObject& getIO() = 0;

private:
  virtual void invoke();
};

class ASWSaveCmd : public TclCmd {
public:
  ASWSaveCmd(Tcl_Interp* interp, const char* cmd_name,
				 const char* usage, int objc) :
	 TclCmd(interp, cmd_name, usage, objc, objc, true) {}

protected:
  virtual IO::IoObject& getIO() = 0;
  virtual const char* getFilename() = 0;

private:
  virtual void invoke();
};

class ASRLoadCmd : public TclCmd {
public:
  ASRLoadCmd(Tcl_Interp* interp, const char* cmd_name,
				 const char* usage, int objc) :
	 TclCmd(interp, cmd_name, usage, objc, objc, true) {}

protected:
  virtual IO::IoObject& getIO() = 0;
  virtual const char* getFilename() = 0;

  virtual void beforeLoadHook();
  virtual void afterLoadHook();

private:
  virtual void invoke();
};

} // end namespace Tcl
 
static const char vcid_stringifycmd_h[] = "$Header$";
#endif // !STRINGIFYCMD_H_DEFINED
