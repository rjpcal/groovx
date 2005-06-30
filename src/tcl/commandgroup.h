///////////////////////////////////////////////////////////////////////
//
// tclcommandgroup.h
//
// Copyright (c) 2004-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Wed Jun  9 09:45:26 2004
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

#ifndef GROOVX_TCL_COMMANDGROUP_H_UTC20050628162421_DEFINED
#define GROOVX_TCL_COMMANDGROUP_H_UTC20050628162421_DEFINED

typedef struct Tcl_Obj Tcl_Obj;
struct Tcl_Interp;

namespace rutz
{
  class file_pos;
  class fstring;
  template <class T> class shared_ptr;
}

namespace tcl
{
  class command;
  class command_group;
  class interpreter;
}

/// Represents a set of overloaded tcl::command objects.
class tcl::command_group
{
public:
  /// Find the named command, if it exists.
  /** Returns null if no such command. */
  static command_group* lookup(tcl::interpreter& interp,
                              const char* name) throw();

  /// Find the named command, after following any namespace aliases.
  /** Returns null if no such command. */
  static command_group* lookup_original(tcl::interpreter& interp,
                                        const char* name) throw();

  /// Find the named command, making a new one if necessary.
  static command_group* make(tcl::interpreter& interp,
                            const rutz::fstring& cmd_name,
                            const rutz::file_pos& src_pos);

  /// Add the given tcl::command to this group's overload list.
  void add(rutz::shared_ptr<tcl::command> p);

  /// Get this group's fully namespace-qualified command name.
  rutz::fstring resolved_name() const;

  /// Returns a string giving the command's proper usage, including overloads.
  rutz::fstring usage() const;

  int invoke_raw(int s_objc, Tcl_Obj *const objv[]) throw();

private:
  class impl;
  friend class impl;
  impl* const rep;

  /// Private constructor since clients should use command_group::make().
  command_group(tcl::interpreter& interp,
               const rutz::fstring& cmd_name,
               const rutz::file_pos& src_pos);

  /// Private destructor since destruction is automated by Tcl.
  ~command_group() throw();

  command_group(const command_group&); // not implemented
  command_group& operator=(const command_group&); // not implemented
};

static const char vcid_groovx_tcl_commandgroup_h_utc20050628162421[] = "$Id$ $HeadURL$";
#endif // !GROOVX_TCL_COMMANDGROUP_H_UTC20050628162421_DEFINED
