///////////////////////////////////////////////////////////////////////
//
// tclpkg.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Jun 14 11:50:23 1999
// written: Tue Feb 22 10:10:25 2000
// $Id$
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

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c TclPkg is a class more managing groups of related \c
 * TclCmd's. It can be used alone, or as a base class for Tcl
 * packages. It provides several facilities:
 *
 *   -- stores a list of \c TclCmd's, and ensures that these are
 *   properly destroyed upon exit from Tcl
 *
 *   -- ensures that the package name is formally "provided" to Tcl so
 *   that other packages can query for its presence
 *
 *   -- provides a function \c makePkgCmdName() to generate a
 *   namespace'd command name from a given command name: the result is
 *   of the form pkg_name::cmd_name (NOTE: the result of this command
 *   is only valid until the next call of \c makePkgCmdName())
 *
 *   -- provides a function \c addCommand() that derived classes can
 *   use (most likely in their constructors) to add \c TclCmd's to the
 *   package
 *
 **/
///////////////////////////////////////////////////////////////////////

class Tcl::TclPkg {
public:
  /** Construct a \c TclPkg with a Tcl interpreter, package name, and
      package version. The version string should be in the form MM.mm
      where MM is major version, and mm is minor version. This
      constructor can also correctly parse a version string such as
      given by the RCS revision tag. */
  TclPkg(Tcl_Interp* interp, const char* name, const char* version);

  /** Virtual destructor ensures proper destruction of base
      classes. This destructor will destroy all \c TclCmd's owned by
      the package. */
  virtual ~TclPkg();

  /// Returns the Tcl interpreter that was passed to the constructor.
  Tcl_Interp* interp();

  /// Returns the package name.
  const char* pkgName();

  /// Returns the package version string.
  const char* version();

  /** Returns a namespace'd command name in the form of
      pkg_name::cmd_name. The result of this function is valid only
      until the next time it is called, so callers should make a copy
      of the result. */
  const char* makePkgCmdName(const char* cmd_name);

  /// Adds \a cmd to the commands managed by the package.
  void addCommand(TclCmd* cmd);

  /// Links the \a var with the Tcl variable \a varName.
  void linkVar(const char* varName, int& var) throw (TclError);
  /// Links \a var with the Tcl variable \a varName.
  void linkVar(const char* varName, double& var) throw (TclError);
  /// Links \a var with the Tcl variable \a varName.
  void linkVar(const char* varName, char*& var) throw (TclError);

  /** Links a copy of \a var with the Tcl variable \a varName. The Tcl
      variable will be read-only.*/
  void linkVarCopy(const char* varName, int var) throw (TclError);
  /** Links a copy of \a var with the Tcl variable \a varName. The Tcl
      variable will be read-only.*/
  void linkVarCopy(const char* varName, double var) throw (TclError);
  /** Links a copy of \a var with the Tcl variable \a varName. The Tcl
      variable will be read-only.*/
  void linkVarCopy(const char* varName, const char* var) throw (TclError);

  /** Links \a var with the Tcl variable \a varName. The Tcl variable
      will be read_only. */
  void linkConstVar(const char* varName, int& var) throw (TclError);
  /** Links \a var with the Tcl variable \a varName. The Tcl variable
      will be read_only. */
  void linkConstVar(const char* varName, double& var) throw (TclError);
  /** Links \a var with the Tcl variable \a varName. The Tcl variable
      will be read_only. */
  void linkConstVar(const char* varName, char*& var) throw (TclError);

private:
  struct Impl;
  Impl* itsImpl;
};

static const char vcid_tclpkg_h[] = "$Header$";
#endif // !TCLPKG_H_DEFINED
