///////////////////////////////////////////////////////////////////////
//
// stringifycmd.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Jun 11 21:43:43 1999
// written: Mon Jul 16 07:06:32 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef STRINGIFYCMD_H_DEFINED
#define STRINGIFYCMD_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TCLCMD_H_DEFINED)
#include "tcl/tclcmd.h"
#endif

namespace IO { class IoObject; }

namespace Tcl
{

class StringifyCmd : public TclCmd {
public:
  StringifyCmd(Tcl_Interp* interp, const char* cmd_name);

  virtual ~StringifyCmd();

protected:
  virtual void invoke(Tcl::Context& ctx);
};

// It is assumed that the string is contained in the last argument
class DestringifyCmd : public TclCmd {
public:
  DestringifyCmd(Tcl_Interp* interp, const char* cmd_name);

  virtual ~DestringifyCmd();

protected:
  virtual void invoke(Tcl::Context& ctx);
};

class WriteCmd : public TclCmd {
public:
  WriteCmd(Tcl_Interp* interp, const char* cmd_name);

  virtual ~WriteCmd();

protected:
  virtual void invoke(Tcl::Context& ctx);
};

class ReadCmd : public TclCmd {
public:
  ReadCmd(Tcl_Interp* interp, const char* cmd_name);

  virtual ~ReadCmd();

protected:
  virtual void invoke(Tcl::Context& ctx);
};

class ASWSaveCmd : public TclCmd {
public:
  ASWSaveCmd(Tcl_Interp* interp, const char* cmd_name);

  virtual ~ASWSaveCmd();

protected:
  virtual void invoke(Tcl::Context& ctx);
};

class ASRLoadCmd : public TclCmd {
public:
  ASRLoadCmd(Tcl_Interp* interp, const char* cmd_name);

  virtual ~ASRLoadCmd();

protected:
  virtual void invoke(Tcl::Context& ctx);
};

} // end namespace Tcl

static const char vcid_stringifycmd_h[] = "$Header$";
#endif // !STRINGIFYCMD_H_DEFINED
