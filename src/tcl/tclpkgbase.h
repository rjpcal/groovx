///////////////////////////////////////////////////////////////////////
//
// tclpkgbase.h
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Mon Jun 14 11:50:23 1999
// written: Wed Mar 19 12:45:43 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLPKGBASE_H_DEFINED
#define TCLPKGBASE_H_DEFINED

struct Tcl_Interp;

namespace Tcl
{
  class Command;
  class Interp;
  class TclError;
  class PkgBase;
}

///////////////////////////////////////////////////////////////////////
//
// Tcl::PkgBase class definition
//
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c Tcl::PkgBase is a class more managing groups of related \c
 * Tcl::Command's. It provides several facilities:
 *
 *   -- stores a list of \c Command's, and ensures that these are
 *   properly destroyed upon exit from Tcl
 *
 *   -- ensures that the package is provided to Tcl so that other
 *   packages can query for its presence
 *
 *   -- provides a \c protected function \c makePkgCmdName() to
 *   generate a namespace'd command name from a given command name:
 *   the result is of the form pkg_name::cmd_name (NOTE: the result of
 *   this command is only valid until the next call of \c
 *   makePkgCmdName())
 *
 *   -- provides a \c protected function \c addCommand() that derived
 *   classes can use to add \c Tcl::Command's to the package
 *
 **/
///////////////////////////////////////////////////////////////////////

class Tcl::PkgBase
{
public:
  /** Construct a \c Tcl::PkgBase with a Tcl interpreter, package
      name, and package version. The version string should be in the
      form MM.mm where MM is major version, and mm is minor
      version. This constructor can also correctly parse a version
      string such as given by the RCS revision tag. */
  PkgBase(Tcl_Interp* interp, const char* name, const char* version);

  /** Virtual destructor ensures proper destruction of base
      classes. This destructor will destroy all \c Command's owned by
      the package. */
  virtual ~PkgBase();

  /** Returns a Tcl status code indicating whether the package
      initialization was successful. */
  int initStatus() const;

  /** Returns a Tcl status code reflecting the combination of this
      package's initStatus() with \a otherStatus. */
  int combineStatus(int otherStatus) const
  {
    return combineStatus(initStatus(), otherStatus);
  }

  /** Returns a Tcl status code representing the combination of the
      current \a status1 with \a status2. If either status represents
      failure, the result will also represent failure, otherwise
      success. */
  static int combineStatus(int status1, int status2);

  /** Return the init status of \a pkg, or return okStatus() if \a pkg
      is null */
  static int initStatus(PkgBase* pkg);

  /// Combine the init statuses of several packages.
  static int initStatus(PkgBase* pkg1, PkgBase* pkg2,
                        PkgBase* pkg3=0, PkgBase* pkg4=0,
                        PkgBase* pkg5=0, PkgBase* pkg6=0)
  {
    return combineStatus(initStatus(pkg1),
           combineStatus(initStatus(pkg2),
           combineStatus(initStatus(pkg3),
           combineStatus(initStatus(pkg4),
           combineStatus(initStatus(pkg5),
                         initStatus(pkg6))))));
  }

  /// Returns the Tcl interpreter that was passed to the constructor.
  Tcl::Interp& interp();

  /// Returns the package name.
  const char* pkgName();

  /// Returns the package version string.
  const char* version();

  /** Causes all of our package's currently defined commands and procedures to
      be imported into the specified other namespace. */
  void namespaceAlias(const char* namesp);

  /** Import all of the commands and procedures defined in the specified
      namespace into our own package namespace. */
  void inherit(const char* namesp);

  /// Does a simple \c Tcl_Eval of \a script using the package's \c Tcl_Interp.
  void eval(const char* script);

  /// Links the \a var with the Tcl variable \a varName.
  void linkVar(const char* varName, int& var);
  /// Links \a var with the Tcl variable \a varName.
  void linkVar(const char* varName, double& var);

  /** Links a copy of \a var with the Tcl variable \a varName. The Tcl
      variable will be read-only.*/
  void linkVarCopy(const char* varName, int var);
  /** Links a copy of \a var with the Tcl variable \a varName. The Tcl
      variable will be read-only.*/
  void linkVarCopy(const char* varName, double var);

  /** Links \a var with the Tcl variable \a varName. The Tcl variable
      will be read_only. */
  void linkConstVar(const char* varName, int& var);
  /** Links \a var with the Tcl variable \a varName. The Tcl variable
      will be read_only. */
  void linkConstVar(const char* varName, double& var);

protected:
  /** Returns a namespace'd command name in the form of
      pkg_name::cmd_name. The result of this function is valid only
      until the next time it is called, so callers should make a copy
      of the result. */
  const char* makePkgCmdName(const char* cmd_name);

  /// Adds \a cmd to the commands managed by the package.
  void addCommand(Command* cmd);

  void setInitStatusError();

private:
  PkgBase(const PkgBase&);
  PkgBase& operator=(const PkgBase&);

  struct Impl;
  Impl* rep;
};

static const char vcid_tclpkgbase_h[] = "$Header$";
#endif // !TCLPKGBASE_H_DEFINED
