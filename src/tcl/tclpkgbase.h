///////////////////////////////////////////////////////////////////////
//
// tclpkg.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Jun 14 11:50:23 1999
// written: Tue Feb  8 18:00:50 2000
// $Id$
//
// This file defines a class TclPkg which can be used alone, or as a
// base class for Tcl packages. It provides several facilities:
//
//   -- stores a list of TclCmd's, and ensures that these are properly
//   destroyed upon exit from Tcl
//
//   -- ensures that the package name is formally "provided" to Tcl so
//   that other packages can query for its presence
//
//   -- provides a function makePkgCmdName to generate a namespace'd
//   command name from a given command name: the result is of the form
//   pkg_name::cmd_name (NOTE: the result of this command is only
//   valid until the next call of makePkgCmdName)
//
//   -- provides a function addCommand that derived classes can use
//   (most likely in their constructors) to add TclCmd's to the package
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLPKG_H_DEFINED
#define TCLPKG_H_DEFINED

struct Tcl_Interp;

namespace Tcl {
  class TclCmd;
  class TclError;
  class TclPkg;
}

///////////////////////////////////////////////////////////////////////
//
// TclPkg class definition
//
///////////////////////////////////////////////////////////////////////

class Tcl::TclPkg {
public:
  TclPkg(Tcl_Interp* interp, const char* name, const char* version);

  virtual ~TclPkg();

  Tcl_Interp* interp();
  const char* pkgName();
  const char* version();

  const char* makePkgCmdName(const char* cmd_name);

  void linkVar(const char* varName, int& var) throw (TclError);
  void linkVar(const char* varName, double& var) throw (TclError);
  void linkVar(const char* varName, char*& var) throw (TclError);
  void linkVarCopy(const char* varName, int var) throw (TclError);
  void linkVarCopy(const char* varName, double var) throw (TclError);
  void linkVarCopy(const char* varName, const char* var) throw (TclError);
  void linkConstVar(const char* varName, int& var) throw (TclError);
  void linkConstVar(const char* varName, double& var) throw (TclError);
  void linkConstVar(const char* varName, char*& var) throw (TclError);

  void addCommand(TclCmd* cmd);

private:
  struct Impl;
  Impl* itsImpl;
};

static const char vcid_tclpkg_h[] = "$Header$";
#endif // !TCLPKG_H_DEFINED
