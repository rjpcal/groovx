///////////////////////////////////////////////////////////////////////
//
// tclscriptapp.h
//
// Copyright (c) 2005-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Mon Jun 27 13:23:46 2005
// commit: $Id$
// $HeadURL$
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

#ifndef GROOVX_TCL_TCLSCRIPTAPP_H_UTC20050627202346_DEFINED
#define GROOVX_TCL_TCLSCRIPTAPP_H_UTC20050627202346_DEFINED

#include "rutz/fstring.h"
#include "rutz/trace.h"

#include <exception>
#include <tcl.h>

namespace Tcl
{
  class ScriptApp;

  struct PackageInfo
  {
    const char*            pkgName;
    Tcl_PackageInitProc*   pkgInitProc;
    const char*            pkgVersion;
    bool                   requiresGui;
  };
}

class Tcl::ScriptApp
{
public:
  ScriptApp(const char* appname_, int argc_, char** argv_) throw();
  ~ScriptApp() throw();

  /// Don't load any packages that require a windowing system.
  /** In particular, don't load Tk. */
  void noWindow() { nowindow = true; }

  /// Set a splash message to be shown at the beginning of run().
  /** The message will shown only if the program is being run
      interactively (i.e. not from a script), and the message will be
      pretty-printed with line-wrapping. Also, empty lines in msg will
      be replaced with a line of hashes ("#####...etc."); to get an
      empty line with no hashes, just pass a line with some invisible
      whitespace (e.g. " \n"). */
  void splash(const char* msg) { splashmsg = msg; }

  /// Specify a directory that should be searched for pkg libraries.
  void pkgDir(const char* dir) { pkgdir = dir; }

  void packages(const PackageInfo* pkgs_) { pkgs = pkgs_; }

  void run();

  int exitStatus() const { return exitcode; }

  static void init_in_macro_only();

  static void handle_exception_in_macro_only(const std::exception* e);

private:
  ScriptApp(const ScriptApp&);
  ScriptApp& operator=(const ScriptApp&);

  rutz::fstring       const appname;
  int                 const argc;
  char**              const argv;
  bool                      minimal;
  bool                      nowindow;
  rutz::fstring             splashmsg;
  rutz::fstring             pkgdir;
  const PackageInfo*        pkgs;
  int                       exitcode;
};

/// Call this macro at the beginning of main().
/** This will define a local variable of type script Tcl::ScriptApp,
    whose variable name is given by app. */
#define GVX_SCRIPT_PROG_BEGIN(app, name, argc, argv)    \
  try                                                   \
  {                                                     \
  GVX_TRACE(name);                                      \
                                                        \
    Tcl::ScriptApp::init_in_macro_only();               \
                                                        \
    Tcl::ScriptApp app(name, argc, argv);


/// Call this macro at the end of main().
#define GVX_SCRIPT_PROG_END(app)                                \
    return app.exitStatus();                                    \
  }                                                             \
  catch (std::exception& err)                                   \
    {                                                           \
      Tcl::ScriptApp::handle_exception_in_macro_only(&err);     \
    }                                                           \
  catch (...)                                                   \
    {                                                           \
      Tcl::ScriptApp::handle_exception_in_macro_only(0);        \
    }                                                           \
  return -1;

static const char vcid_groovx_tcl_tclscriptapp_h_utc20050627202346[] = "$Id$ $URL$";
#endif // !GROOVX_TCL_TCLSCRIPTAPP_H_UTC20050627202346DEFINED
