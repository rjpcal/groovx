/** @file tcl/pkg.h tcl package class, holds a set of commands, wraps
    calls to Tcl_PkgProvide(), etc. */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Tue Jun 15 12:33:59 1999
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
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

#ifndef GROOVX_TCL_PKG_H_UTC20050628162421_DEFINED
#define GROOVX_TCL_PKG_H_UTC20050628162421_DEFINED

#include "tcl/makecmd.h"

#include "rutz/fileposition.h"

#include <functional>

struct Tcl_Interp;

namespace rutz
{
  struct file_pos;
}

namespace tcl
{
  class command;
  class interpreter;
  class pkg;

  const int NO_EXPORT = 1 << 0;
}

///////////////////////////////////////////////////////////////////////
//
// tcl::pkg class definition
//
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
/**

    \c tcl::pkg is a class more managing groups of related \c
    tcl::command's. It provides several facilities:

    -- stores a list of \c tcl::command's, and ensures that these are
       properly destroyed upon exit from Tcl

    -- ensures that the package is provided to Tcl so that other
       packages can query for its presence

    -- provides a set of functions to define Tcl commands from C++
       functors

 **/
///////////////////////////////////////////////////////////////////////

class tcl::pkg
{
private:
  /// Private constructor.
  /** Clients should use init() instead. */
  pkg(Tcl_Interp* interp, const char* name, const char* version);

  /// Destructor destroys all \c tcl::command's owned by the package.
  ~pkg() noexcept;

public:
  static const int STATUS_OK;
  static const int STATUS_ERR;

  pkg(const pkg&) = delete;
  pkg& operator=(const pkg&) = delete;

  /// Call from within a extern "C" Pkg_Init(Tcl_Interp*) function
  /** Internally creates a new tcl::pkg object from a package name and
      version. The version string should be in the form MM.mm where MM
      is major version, and mm is minor version. Then calls your
      setup() function on it. If the package's status is OK, then this
      does the relevant Tcl_PkgProvide and returns TCL_OK. Otherwise,
      it returns TCL_ERROR. You can directly return the status code
      from your Pkg_init() function. */
  static int init(Tcl_Interp* interp, const char* name, const char* version,
                  const std::function<void(tcl::pkg*)>& setup);

  typedef void (exit_callback)();

  /// Specify a function to be called when the package is destroyed.
  /** (Package destruction typically occurs at application exit, when
      the Tcl interpreter and all associated objects are
      destroyed.) */
  void on_exit(exit_callback* callback);

  /// Looks up the tcl::pkg associated with pkgname, and destroys it.
  /** This is intended to be called from pkg_Unload procedures called
      by Tcl when a dynamic library is unloaded. The return value can
      be returned as the return value of the pkg_Unload procedure; it
      will be TCL_OK (1) if the tcl::pkg was successfully found and
      destroyed and TCL_ERROR (0) otherwise. */
  static int destroy_on_unload(Tcl_Interp* interp, const char* pkgname);

  /// Find a package given its name and version.
  /** If the package is not already loaded, this function will attempt
      to "require" the package. If a null pointer is passed to version
      (the default), then any version will be acceptable. If no
      suitable package cannot be found or loaded, a null pointer will
      be returned. */
  static pkg* lookup(tcl::interpreter& interp,
                     const char* name, const char* version = 0) noexcept;

  /// Returns the Tcl interpreter that was passed to the constructor.
  tcl::interpreter& interp() noexcept;

  /// Returns the package's "namespace name".
  /** Note that the "namespace name" will be the same as the "package
      name" except possibly for capitalization. The "namespace name"
      is the name of the namespace that is used as the default prefix
      all commands contained in the package. */
  const char* namesp_name() noexcept;

  /// Return the package's "package name".
  /** Note that the "package name" will be the same as the "namespace
      name" except possibly for capitalization. The "package name" is
      the name that is passed to Tcl_PkgProvide() and
      Tcl_PkgProvide(), and has a well-defined capitalization scheme:
      first character uppercase, all remaining letters lowercase. */
  const char* pkg_name() const noexcept;

  /// Returns the package version string.
  const char* version() const noexcept;

  /// Export commands into a different namespace.
  /** Causes all of our package's currently defined commands and
      procedures to be imported into the specified other namespace. If
      pattern is different from the default value of "*", then only
      commands whose names match pattern according to glob rules will
      be aliased into the other namespace. */
  void namesp_alias(const char* namesp, const char* pattern = "*");

  /// Import commands from a different namespace.
  /** Import all of the commands and procedures defined in the
      specified namespace into our own package namespace. If pattern
      is different from the default value of "*", then only commands
      whose names match pattern according to glob rules will be
      imported into our own package namespace. */
  void inherit_namesp(const char* namesp, const char* pattern = "*");

  /// Import all commands and procedures defined in the named pkg.
  /** If the named pkg has not yet been loaded, this function will
      attempt to load it via loookup(). If a null pointer is passed to
      version (the default), then any version will be acceptable. */
  void inherit_pkg(const char* name, const char* version = 0);

  /// Evaluates \a script using the package's \c Tcl_Interp.
  void eval(const char* script);

  /// Links the \a var with the Tcl variable \a var_name.
  void link_var(const char* var_name, int& var);

  /// Links \a var with the Tcl variable \a var_name.
  void link_var(const char* var_name, double& var);

  /// Links a copy of \a var with the Tcl variable \a var_name.
  /** The Tcl variable will be read-only.*/
  void link_var_copy(const char* var_name, int var);

  /// Links a copy of \a var with the Tcl variable \a var_name.
  /** The Tcl variable will be read-only.*/
  void link_var_copy(const char* var_name, double var);

  /// Links \a var with the Tcl variable \a var_name.
  /** The Tcl variable will be read_only. */
  void link_var_const(const char* var_name, int& var);

  ///Links \a var with the Tcl variable \a var_name.
  /** The Tcl variable will be read_only. */
  void link_var_const(const char* var_name, double& var);

  class cmd_name_init
  {
  public:
    const char* name;
    int flags;

    cmd_name_init(const char* n, int f = 0) : name(n), flags(f) {}
  };

  static cmd_name_init no_export(const char* n) { return cmd_name_init(n, NO_EXPORT); }

  template <class Func>
  inline void def(cmd_name_init cmd_name, const char* usage, Func f,
                  const rutz::file_pos& src_pos)
  {
    make_command(interp(), f, make_pkg_cmd_name(cmd_name),
                 usage, src_pos);
  }

  template <class Func>
  inline void def_vec(cmd_name_init cmd_name, const char* usage, Func f,
                      unsigned int keyarg /*default is 1*/,
                      const rutz::file_pos& src_pos)
  {
    make_vec_command(interp(), f, make_pkg_cmd_name(cmd_name),
                     usage, keyarg, src_pos);
  }

  template <class Func>
  inline void def_raw(cmd_name_init cmd_name, const arg_spec& spec,
                      const char* usage, Func f,
                      const rutz::file_pos& src_pos)
  {
    make_generic_command(interp(), f, make_pkg_cmd_name(cmd_name),
                         usage, spec, src_pos);
  }

  template <class Func>
  inline void def_vec_raw(cmd_name_init cmd_name, const arg_spec& spec,
                          const char* usage, Func f,
                          unsigned int keyarg /*default is 1*/,
                          const rutz::file_pos& src_pos)
  {
    make_generic_vec_command(interp(), f, make_pkg_cmd_name(cmd_name),
                             usage, spec, keyarg, src_pos);
  }

  template <class C>
  void def_action(cmd_name_init cmd_name, void (C::* action_func) (),
                  const rutz::file_pos& src_pos)
  {
    def_vec( cmd_name, action_usage, action_func, 1, src_pos );
  }

  template <class C>
  void def_action(cmd_name_init cmd_name, void (C::* action_func) () const,
                  const rutz::file_pos& src_pos)
  {
    def_vec( cmd_name, action_usage, action_func, 1, src_pos );
  }

  template <class C, class T>
  void def_getter(cmd_name_init cmd_name, T (C::* getter_func) () const,
                  const rutz::file_pos& src_pos)
  {
    def_vec( cmd_name, getter_usage, getter_func, 1, src_pos );
  }

  template <class C, class T>
  void def_setter(cmd_name_init cmd_name, void (C::* setter_func) (T),
                  const rutz::file_pos& src_pos)
  {
    def_vec( cmd_name, setter_usage, setter_func, 1, src_pos );
  }

  template <class C, class T>
  void def_get_set(cmd_name_init cmd_name,
                   T (C::* getter_func) () const,
                   void (C::* setter_func) (T),
                   const rutz::file_pos& src_pos)
  {
    def_getter( cmd_name, getter_func, src_pos );
    def_setter( cmd_name, setter_func, src_pos );
  }

  /// Control whether packages should be verbose as they start up.
  static void verbose_init(bool verbose) noexcept;

private:

  /** Returns a namespace'd command name in the form of
      pkg_name::cmd_name. The result of this function is valid only
      until the next time it is called, so callers should make a copy
      of the result. This function also has the side effect of setting
      up a Tcl namespace export pattern for the named command, if
      flags doesn't include NO_EXPORT. */
  const char* make_pkg_cmd_name(cmd_name_init cmd_name);

  static const char* const action_usage;
  static const char* const getter_usage;
  static const char* const setter_usage;

  struct impl;
  friend struct impl;
  impl* rep;
};

#endif // !GROOVX_TCL_PKG_H_UTC20050628162421_DEFINED
