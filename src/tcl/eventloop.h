///////////////////////////////////////////////////////////////////////
//
// tclmain.h
//
// Copyright (c) 2002-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Mon Jul 22 16:32:01 2002
// commit: $Id$
// $HeadURL$
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

#ifndef GROOVX_TCL_EVENTLOOP_H_UTC20050628162420_DEFINED
#define GROOVX_TCL_EVENTLOOP_H_UTC20050628162420_DEFINED

namespace rutz
{
  class fstring;
}

namespace tcl
{
  class interpreter;
  class event_loop;
}

/// Singleton class that operates the main Tcl event loop.
/** Its responsibilities include gathering text commands from standard
    input, dispatching window-system events, and dispatching
    timer-callbacks and idle-callbacks. */
class tcl::event_loop
{
public:
  event_loop(int argc, char** argv, bool nowindow);
  ~event_loop();

  static bool is_interactive();

  static tcl::interpreter& interp();

  static void run();

  /// Get the application's number of command-line arguments.
  static int argc();

  /// Get the application's command-line arguments.
  static const char* const* argv();

  /// Get the whole command-line as a single string.
  static rutz::fstring command_line();

private:
  event_loop(const event_loop&);
  event_loop& operator=(const event_loop&);
};

static const char vcid_groovx_tcl_eventloop_h_utc20050628162420[] = "$Id$ $HeadURL$";
#endif // !GROOVX_TCL_EVENTLOOP_H_UTC20050628162420_DEFINED