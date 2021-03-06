/** @file tcl/scriptapp.h helper class used in main() to initialize
    and run a scripting application */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2005-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Mon Jun 27 13:23:46 2005
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [https://github.com/rjpcal/groovx]
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

#ifndef GROOVX_TCL_SCRIPTAPP_H_UTC20050628162420_DEFINED
#define GROOVX_TCL_SCRIPTAPP_H_UTC20050628162420_DEFINED

#include "rutz/fstring.h"
#include "rutz/trace.h"

#include <exception>
#include <tcl.h>

namespace tcl
{
  class script_app;

  struct package_info
  {
    const char*            name;
    Tcl_PackageInitProc*   init_proc;
    const char*            version;
    bool                   requires_gui;
  };
}

/// Use inside main() to initialize and run a scripting application.
class tcl::script_app
{
public:
  script_app(const char* appname_, int argc_, char** argv_) noexcept;
  ~script_app() noexcept;

  /// Don't load any packages that require a windowing system.
  /** In particular, don't load Tk. */
  void no_gui() { m_nowindow = true; }

  /// Set a splash message to be shown at the beginning of run().
  /** The message will shown only if the program is being run
      interactively (i.e. not from a script), and the message will be
      pretty-printed with line-wrapping. Also, empty lines in msg will
      be replaced with a line of hashes ("#####...etc."); to get an
      empty line with no hashes, just pass a line with some invisible
      whitespace. */
  void splash(const char* msg) { m_splashmsg = msg; }

  /// Specify a directory that should be searched for pkg libraries.
  void pkg_dir(const char* dir) { m_pkgdir = dir; }

  void packages(const package_info* pkgs_) { m_pkgs = pkgs_; }

  void run();

  int exit_status() const { return m_exitcode; }

  static void init_in_macro_only();

  static void handle_exception_in_macro_only(const std::exception* e);

private:
  script_app(const script_app&); // not implemented
  script_app& operator=(const script_app&); // not implemented

  rutz::fstring       const m_appname;
  int                       m_script_argc;
  char**                    m_script_argv;
  bool                      m_minimal;
  bool                      m_nowindow;
  rutz::fstring             m_splashmsg;
  rutz::fstring             m_pkgdir;
  const package_info*       m_pkgs;
  int                       m_exitcode;
};

/// Call this macro at the beginning of main().
/** This will define a local variable of type script tcl::script_app,
    whose variable name is given by app. */
#define GVX_SCRIPT_PROG_BEGIN(app, name, argc, argv)    \
  try                                                   \
  {                                                     \
  GVX_TRACE(name);                                      \
                                                        \
    tcl::script_app::init_in_macro_only();              \
                                                        \
    tcl::script_app app(name, argc, argv);


/// Call this macro at the end of main().
#define GVX_SCRIPT_PROG_END(app)                                \
    return app.exit_status();                                   \
  }                                                             \
  catch (std::exception& err)                                   \
    {                                                           \
      tcl::script_app::handle_exception_in_macro_only(&err);    \
    }                                                           \
  catch (...)                                                   \
    {                                                           \
      tcl::script_app::handle_exception_in_macro_only(0);       \
    }                                                           \
  return -1;

#endif // !GROOVX_TCL_SCRIPTAPP_H_UTC20050628162420_DEFINED
