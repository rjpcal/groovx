///////////////////////////////////////////////////////////////////////
//
// stringifycmd.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Jun 11 21:43:43 1999
// written: Wed Jul 11 19:44:05 2001
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
               const char* usage, int objc);

  virtual ~StringifyCmd();

protected:
  virtual IO::IoObject& getIO(Context& ctx) = 0;

private:
  virtual void invoke(Context& ctx);
};

// It is assumed that the string is contained in the last argument
class DestringifyCmd : public TclCmd {
public:
  DestringifyCmd(Tcl_Interp* interp, const char* cmd_name,
                 const char* usage, int objc);

  virtual ~DestringifyCmd();

protected:
  virtual IO::IoObject& getIO(Context& ctx) = 0;

private:
  virtual void invoke(Context& ctx);
};

class WriteCmd : public TclCmd {
public:
  WriteCmd(Tcl_Interp* interp, const char* cmd_name,
           const char* usage, int objc);

  virtual ~WriteCmd();

protected:
  virtual IO::IoObject& getIO(Context& ctx) = 0;

private:
  virtual void invoke(Context& ctx);
};

class ReadCmd : public TclCmd {
public:
  ReadCmd(Tcl_Interp* interp, const char* cmd_name,
          const char* usage, int objc);

  virtual ~ReadCmd();

protected:
  virtual IO::IoObject& getIO(Context& ctx) = 0;

private:
  virtual void invoke(Context& ctx);
};

class ASWSaveCmd : public TclCmd {
public:
  ASWSaveCmd(Tcl_Interp* interp, const char* cmd_name,
             const char* usage, int objc);

  virtual ~ASWSaveCmd();

protected:
  virtual IO::IoObject& getIO(Context& ctx) = 0;
  virtual const char* getFilename(Context& ctx) = 0;

private:
  virtual void invoke(Context& ctx);
};

class ASRLoadCmd : public TclCmd {
public:
  ASRLoadCmd(Tcl_Interp* interp, const char* cmd_name,
             const char* usage, int objc);

  virtual ~ASRLoadCmd();

protected:
  virtual IO::IoObject& getIO(Context& ctx) = 0;
  virtual const char* getFilename(Context& ctx) = 0;

  virtual void beforeLoadHook();
  virtual void afterLoadHook();

private:
  virtual void invoke(Context& ctx);
};

} // end namespace Tcl

static const char vcid_stringifycmd_h[] = "$Header$";
#endif // !STRINGIFYCMD_H_DEFINED
