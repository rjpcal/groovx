///////////////////////////////////////////////////////////////////////
//
// tclpkg.h
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Tue Jun 15 12:33:59 1999
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLPKG_H_DEFINED
#define TCLPKG_H_DEFINED

#include "tcl/tclfunctor.h"

struct Tcl_Interp;

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
 *
 * \c Tcl::Pkg is a class more managing groups of related \c
 * Tcl::Command's. It provides several facilities:
 *
 *   -- stores a list of \c Command's, and ensures that these are
 *   properly destroyed upon exit from Tcl
 *
 *   -- ensures that the package is provided to Tcl so that other
 *   packages can query for its presence
 *
 *   -- provides a set of functions to define Tcl commands from C++ functors
 *
 **/
///////////////////////////////////////////////////////////////////////

class Tcl::Pkg
{
public:
  /** Construct a \c Tcl::Pkg with a Tcl interpreter, package
      name, and package version. The version string should be in the
      form MM.mm where MM is major version, and mm is minor
      version. This constructor can also correctly parse a version
      string such as given by the RCS revision tag. */
  Pkg(Tcl_Interp* interp, const char* name, const char* version);

  /// Function type to be passed to Tcl::Pkg::create.
  typedef void (SetupCallback)(Tcl::Pkg*);

  typedef void (ExitCallback)();

  /// Creates a new Tcl::Pkg, then calls the setup callback with that argument.
  /** There are several advantages of this scheme over just creating a
      Tcl::Pkg directly. First of all, Tcl::Pkg objects are typically
      created within *_Init() functions. These functions are called from
      within the Tcl library, and therefore must not transmit any
      exceptions. But, putting a try/catch block in every *_Init() function
      is a lot of unneeded boilerplate. In constrast, this create()
      function takes care of catching all exceptions and translating them
      into an appropriate return code. For example:

      \code
      namespace
      {
        void Foo_Setup(Tcl::Pkg* pkg)
        {
          pkg->def( ... );
        }
      }

      extern "C" int Foo_Init(Tcl_Interp* interp)
      {
        return Tcl::Pkg::create(interp, "Foo", "1.1", &Foo_Setup);
      }

      \endcode

      Normally, create() will assume that package initialization succeeded
      and return TCL_OK.

      However, if either (1) an exception is caught, or (2) the
      Tcl_Interp's result is non-empty, then create() will assume that
      package initialization failed, and will return TCL_ERROR.
  */
  static int create(Tcl_Interp* interp, const char* name, const char* version,
                    SetupCallback* setup, ExitCallback* ex = 0);

private:
  /** Virtual destructor ensures proper destruction of base
      classes. This destructor will destroy all \c Command's owned by
      the package. */
  virtual ~Pkg() throw();

public:
  /// Specify a function to be called when the package is destroyed.
  /** (Package destruction typically occurs at application exit, when the
      Tcl interpreter and all associated objects are destroyed.) */
  void onExit(ExitCallback* callback);

  /// Find a package given its name and version.
  /** If the package is not already loaded, this function will attempt to
      "require" the package. If a null pointer is passed to version (the
      default), then any version will be acceptable. If no suitable package
      cannot be found or loaded, a null pointer will be returned. */
  static Pkg* lookup(Tcl_Interp* interp,
                     const char* name, const char* version = 0) throw();

  /** Returns a Tcl status code indicating whether the package
      initialization was successful. */
  int initStatus() const throw();

  /// Mark the package as having failed its initialization.
  void setInitStatusError() throw();

  /// Returns the Tcl interpreter that was passed to the constructor.
  Tcl::Interp& interp() throw();

  /// Returns the package's "namespace name".
  /** Note that the "namespace name" will be the same as the "package name"
      except possibly for capitalization. The "namespace name" is the name
      of the namespace that is used as the default prefix all commands
      contained in the package. */
  const char* namespName() throw();

  /// Return the package's "package name".
  /** Note that the "package name" will be the same as the "namespace name"
      except possibly for capitalization. The "package name" is the name
      that is passed to Tcl_PkgProvide() and Tcl_PkgProvide(), and has a
      well-defined capitalization scheme: first character uppercase, all
      remaining letters lowercase. */
  const char* pkgName() throw();

  /// Returns the package version string.
  const char* version() throw();

  /** Causes all of our package's currently defined commands and procedures to
      be imported into the specified other namespace. */
  void namespaceAlias(const char* namesp) throw();

  /** Import all of the commands and procedures defined in the specified
      namespace into our own package namespace. */
  void inherit(const char* namesp) throw();

  /// Import all of the commands and procedures defined in the named pkg.
  /** If the named pkg has not yet been loaded, this function will attempt
      to load it via loookup(). If a null pointer is passed to version (the
      default), then any version will be acceptable. */
  void inheritPkg(const char* name, const char* version = 0) throw();

  /// Does a simple \c Tcl_Eval of \a script using the package's \c Tcl_Interp.
  void eval(const char* script) throw();

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


  template <class Func>
  inline void def(const char* cmd_name, const char* usage, Func f)
  {
    addCommand( makeCmd(interp(), f, makePkgCmdName(cmd_name), usage) );
  }

  template <class Func>
  inline void defVec(const char* cmd_name, const char* usage, Func f,
                     unsigned int keyarg=1)
  {
    addCommand( makeVecCmd(interp(), f, makePkgCmdName(cmd_name),
                           usage, keyarg) );
  }

  template <class Func>
  inline void defRaw(const char* cmd_name, unsigned int nargs,
                     const char* usage, Func f)
  {
    addCommand( makeGenericCmd(interp(), f, makePkgCmdName(cmd_name),
                               usage, nargs) );
  }

  template <class Func>
  inline void defVecRaw(const char* cmd_name, unsigned int nargs,
                        const char* usage, Func f,
                        unsigned int keyarg=1)
  {
    addCommand( makeGenericVecCmd(interp(), f, makePkgCmdName(cmd_name),
                                  usage, nargs, keyarg) );
  }

  template <class C>
  void defAction(const char* cmd_name, void (C::* actionFunc) ())
  {
    defVec( cmd_name, actionUsage(""), actionFunc );
  }

  template <class C>
  void defAction(const char* cmd_name, void (C::* actionFunc) () const)
  {
    defVec( cmd_name, actionUsage(""), actionFunc );
  }

  template <class C, class T>
  void defGetter(const char* cmd_name, const char* usage,
                 T (C::* getterFunc) () const)
  {
    defVec( cmd_name, getterUsage(usage), getterFunc );
  }

  template <class C, class T>
  void defGetter(const char* cmd_name, T (C::* getterFunc) () const)
  {
    defVec( cmd_name, getterUsage(""), getterFunc );
  }

  template <class C, class T>
  void defSetter(const char* cmd_name, const char* usage,
                 void (C::* setterFunc) (T))
  {
    defVec( cmd_name, setterUsage(usage), setterFunc );
  }

  template <class C, class T>
  void defSetter(const char* cmd_name, void (C::* setterFunc) (T))
  {
    defVec( cmd_name, setterUsage(""), setterFunc );
  }

  template <class C, class T>
  void defAttrib(const char* cmd_name,
                 T (C::* getterFunc) () const,
                 void (C::* setterFunc) (T))
  {
    defGetter( cmd_name, getterFunc );
    defSetter( cmd_name, setterFunc );
  }

private:
  Pkg(const Pkg&); // not implemented
  Pkg& operator=(const Pkg&); // not implemented

  /** Returns a namespace'd command name in the form of
      pkg_name::cmd_name. The result of this function is valid only
      until the next time it is called, so callers should make a copy
      of the result. */
  const char* makePkgCmdName(const char* cmd_name);

  /// Adds \a cmd to the commands managed by the package.
  void addCommand(Command* cmd);

  static const char* actionUsage(const char* usage);
  static const char* getterUsage(const char* usage);
  static const char* setterUsage(const char* usage);

  struct Impl;
  friend struct Impl;
  Impl* rep;
};

static const char vcid_tclpkg_h[] = "$Header$";
#endif // !TCLPKG_H_DEFINED
