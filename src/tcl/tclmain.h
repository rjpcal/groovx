///////////////////////////////////////////////////////////////////////
//
// tclmain.h
//
// Copyright (c) 2002-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Mon Jul 22 16:32:01 2002
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

#ifndef TCLMAIN_H_DEFINED
#define TCLMAIN_H_DEFINED

namespace rutz
{
  class fstring;
}

namespace Tcl
{
  class Interp;
  class Main;
}

/// Singleton class that operates the main Tcl event loop.
/** Its responsibilities include gathering text commands from standard
    input, dispatching window-system events, and dispatching
    timer-callbacks and idle-callbacks. */
class Tcl::Main
{
public:
  Main(int argc, char** argv);
  ~Main();

  static bool isInteractive();

  static Tcl::Interp& interp();

  static void run();

  /// Get the application's number of command-line arguments.
  static int argc();

  /// Get the application's command-line arguments.
  static const char* const* argv();

  /// Get the whole command-line as a single string.
  static rutz::fstring commandLine();

private:
  Main(const Main&);
  Main& operator=(const Main&);
};

static const char vcid_tclmain_h[] = "$Id$ $URL$";
#endif // !TCLMAIN_H_DEFINED
