///////////////////////////////////////////////////////////////////////
//
// tclpkg.h
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Tue Jun 15 12:33:59 1999
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLPKG_H_DEFINED
#define TCLPKG_H_DEFINED

#include "tcl/tclfunctor.h"

#include "util/fileposition.h"

struct Tcl_Interp;

namespace rutz
{
  struct file_pos;
}

namespace Tcl
{
  class Command;
  class Interp;
  class Pkg;
}

///////////////////////////////////////////////////////////////////////
//
// Tcl::Pkg class definition
//
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
/**

    \c Tcl::Pkg is a class more managing groups of related \c
    Tcl::Command's. It provides several facilities:

    -- stores a list of \c Command's, and ensures that these are
       properly destroyed upon exit from Tcl

    -- ensures that the package is provided to Tcl so that other
       packages can query for its presence

    -- provides a set of functions to define Tcl commands from C++
       functors

 **/
///////////////////////////////////////////////////////////////////////

class Tcl::Pkg
{
private:
  /// Private constructor.
  /** Clients should use the PKG_CREATE macro instead. */
  Pkg(Tcl_Interp* interp, const char* name, const char* version);

  /// Destructor destroys all \c Command's owned by the package.
  ~Pkg() throw();

public:
  /// Don't call this directly! Use the PKG_CREATE macro instead.
  static Pkg* createInMacro(Tcl_Interp* interp,
                            const char* name, const char* version)
  {
    return new Pkg(interp, name, version);
  }

  typedef void (ExitCallback)();

  /// Specify a function to be called when the package is destroyed.
  /** (Package destruction typically occurs at application exit, when
      the Tcl interpreter and all associated objects are
      destroyed.) */
  void onExit(ExitCallback* callback);

  /// Looks up the Tcl::Pkg associated with pkgname, and destroys it.
  /** This is intended to be called from pkg_Unload procedures called
      by Tcl when a dynamic library is unloaded. The return value can
      be returned as the return value of the pkg_Unload procedure; it
      will be TCL_OK (1) if the Tcl::Pkg was successfully found and
      destroyed and TCL_ERROR (0) otherwise. */
  static int unloadDestroy(Tcl_Interp* interp, const char* pkgname);

  /// Find a package given its name and version.
  /** If the package is not already loaded, this function will attempt
      to "require" the package. If a null pointer is passed to version
      (the default), then any version will be acceptable. If no
      suitable package cannot be found or loaded, a null pointer will
      be returned. */
  static Pkg* lookup(Tcl::Interp& interp,
                     const char* name, const char* version = 0) throw();

  /** Returns a Tcl status code indicating whether the package
      initialization was successful. */
  int initStatus() const throw();

  /// Mark the package as having failed its initialization.
  void setInitStatusError() throw();

  /// Returns the Tcl interpreter that was passed to the constructor.
  Tcl::Interp& interp() throw();

  /// Trap a live exception, and leave a message in the Tcl_Interp's result.
  void handleLiveException(const rutz::file_pos& pos) throw();

  /// Returns the package's "namespace name".
  /** Note that the "namespace name" will be the same as the "package
      name" except possibly for capitalization. The "namespace name"
      is the name of the namespace that is used as the default prefix
      all commands contained in the package. */
  const char* namespName() throw();

  /// Return the package's "package name".
  /** Note that the "package name" will be the same as the "namespace
      name" except possibly for capitalization. The "package name" is
      the name that is passed to Tcl_PkgProvide() and
      Tcl_PkgProvide(), and has a well-defined capitalization scheme:
      first character uppercase, all remaining letters lowercase. */
  const char* pkgName() const throw();

  /// Returns the package version string.
  const char* version() const throw();

  /// Export commands into a different namespace.
  /** Causes all of our package's currently defined commands and
      procedures to be imported into the specified other namespace. If
      pattern is different from the default value of "*", then only
      commands whose names match pattern according to glob rules will
      be aliased into the other namespace. */
  void namespaceAlias(const char* namesp,
                      const char* pattern = "*") throw();

  /// Import commands from a different namespace.
  /** Import all of the commands and procedures defined in the
      specified namespace into our own package namespace. If pattern
      is different from the default value of "*", then only commands
      whose names match pattern according to glob rules will be
      imported into our own package namespace. */
  void inherit(const char* namesp,
               const char* pattern = "*") throw();

  /// Import all commands and procedures defined in the named pkg.
  /** If the named pkg has not yet been loaded, this function will
      attempt to load it via loookup(). If a null pointer is passed to
      version (the default), then any version will be acceptable. */
  void inheritPkg(const char* name, const char* version = 0) throw();

  /// Evaluates \a script using the package's \c Tcl_Interp.
  void eval(const char* script) throw();

  /// Links the \a var with the Tcl variable \a varName.
  void linkVar(const char* varName, int& var);

  /// Links \a var with the Tcl variable \a varName.
  void linkVar(const char* varName, double& var);

  /// Links a copy of \a var with the Tcl variable \a varName.
  /** The Tcl variable will be read-only.*/
  void linkVarCopy(const char* varName, int var);

  /// Links a copy of \a var with the Tcl variable \a varName.
  /** The Tcl variable will be read-only.*/
  void linkVarCopy(const char* varName, double var);

  /// Links \a var with the Tcl variable \a varName.
  /** The Tcl variable will be read_only. */
  void linkVarConst(const char* varName, int& var);

  ///Links \a var with the Tcl variable \a varName.
  /** The Tcl variable will be read_only. */
  void linkVarConst(const char* varName, double& var);


  template <class Func>
  inline void def(const char* cmd_name, const char* usage, Func f,
                  const rutz::file_pos& src_pos)
  {
    makeCmd(interp(), f, makePkgCmdName(cmd_name),
            usage, src_pos);
  }

  template <class Func>
  inline void defVec(const char* cmd_name, const char* usage, Func f,
                     unsigned int keyarg /*default is 1*/,
                     const rutz::file_pos& src_pos)
  {
    makeVecCmd(interp(), f, makePkgCmdName(cmd_name),
               usage, keyarg, src_pos);
  }

  template <class Func>
  inline void defRaw(const char* cmd_name, unsigned int nargs,
                     const char* usage, Func f,
                     const rutz::file_pos& src_pos)
  {
    makeGenericCmd(interp(), f, makePkgCmdName(cmd_name),
                   usage, nargs, src_pos);
  }

  template <class Func>
  inline void defVecRaw(const char* cmd_name, unsigned int nargs,
                        const char* usage, Func f,
                        unsigned int keyarg /*default is 1*/,
                        const rutz::file_pos& src_pos)
  {
    makeGenericVecCmd(interp(), f, makePkgCmdName(cmd_name),
                      usage, nargs, keyarg, src_pos);
  }

  template <class C>
  void defAction(const char* cmd_name, void (C::* actionFunc) (),
                 const rutz::file_pos& src_pos)
  {
    defVec( cmd_name, actionUsage, actionFunc, 1, src_pos );
  }

  template <class C>
  void defAction(const char* cmd_name, void (C::* actionFunc) () const,
                 const rutz::file_pos& src_pos)
  {
    defVec( cmd_name, actionUsage, actionFunc, 1, src_pos );
  }

  template <class C, class T>
  void defGetter(const char* cmd_name, T (C::* getterFunc) () const,
                 const rutz::file_pos& src_pos)
  {
    defVec( cmd_name, getterUsage, getterFunc, 1, src_pos );
  }

  template <class C, class T>
  void defSetter(const char* cmd_name, void (C::* setterFunc) (T),
                 const rutz::file_pos& src_pos)
  {
    defVec( cmd_name, setterUsage, setterFunc, 1, src_pos );
  }

  template <class C, class T>
  void defAttrib(const char* cmd_name,
                 T (C::* getterFunc) () const,
                 void (C::* setterFunc) (T),
                 const rutz::file_pos& src_pos)
  {
    defGetter( cmd_name, getterFunc, src_pos );
    defSetter( cmd_name, setterFunc, src_pos );
  }

  /// Control whether packages should be verbose as they start up.
  static void verboseInit(bool verbose) throw();

  /// Called just prior to returning from the *_Init function.
  void finishInit() const;

private:
  Pkg(const Pkg&); // not implemented
  Pkg& operator=(const Pkg&); // not implemented

  /** Returns a namespace'd command name in the form of
      pkg_name::cmd_name. The result of this function is valid only
      until the next time it is called, so callers should make a copy
      of the result. */
  const char* makePkgCmdName(const char* cmd_name);

  static const char* const actionUsage;
  static const char* const getterUsage;
  static const char* const setterUsage;

  struct Impl;
  friend struct Impl;
  Impl* rep;
};

/*
  These macros make it slightly more convenient to make sure that
  *_Init() package initialization functions don't leak any exceptions
  (as they are called directly from C code within the Tcl core).
 */

/// This macro should go at the top of each *_Init() function.
/** Constructs a \c Tcl::Pkg with a Tcl interpreter, package name, and
    package version. The version string should be in the form MM.mm
    where MM is major version, and mm is minor version. This
    constructor can also correctly parse a version string such as
    given by the RCS revision tag. */
#define PKG_CREATE(interp, pkgname, pkgversion)                     \
                                                                    \
Tcl::Pkg* pkg = 0;                                                  \
                                                                    \
try { pkg = Tcl::Pkg::createInMacro(interp, pkgname, pkgversion); } \
catch (...) { return 1; }                                           \
                                                                    \
try                                                                 \
{


/// This macro should go at the end of each *_Init() function.
#define PKG_RETURN                              \
  pkg->finishInit();                            \
  return pkg->initStatus();                     \
}                                               \
catch(...)                                      \
{                                               \
  pkg->handleLiveException(SRC_POS);            \
}                                               \
return 1;


static const char vcid_tclpkg_h[] = "$Id$ $URL$";
#endif // !TCLPKG_H_DEFINED
