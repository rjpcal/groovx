/** @file tcl/commandgroup.h represents a set of overloaded
    tcl::command objects */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2004-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Wed Jun  9 09:45:26 2004
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

#ifndef GROOVX_TCL_COMMANDGROUP_H_UTC20050628162421_DEFINED
#define GROOVX_TCL_COMMANDGROUP_H_UTC20050628162421_DEFINED

#include <functional>
#include <memory>

typedef struct Tcl_Obj Tcl_Obj;
struct Tcl_Interp;

namespace rutz
{
  struct file_pos;
  class fstring;
}

namespace tcl
{
  class arg_dispatcher;
  class arg_spec;
  class call_context;
  class command;
  class command_group;
  class interpreter;
}

/// Represents a set of overloaded tcl::command objects.
class tcl::command_group
{
public:
  /// Find the named command, if it exists.
  /** Returns null if no such command. DO NOT DELETE the pointer
      returned from this function! Its lifetime is managed internally
      by tcl. */
  static command_group* lookup(tcl::interpreter& interp,
                               const char* name) noexcept;

  /// Find the named command, after following any namespace aliases.
  /** Returns null if no such command. DO NOT DELETE the pointer
      returned from this function! Its lifetime is managed internally
      by tcl. */
  static command_group* lookup_original(tcl::interpreter& interp,
                                        const char* name) noexcept;

  /// Build a new tcl::command object that will be hooked into a tcl::command_group.
  /** If there is already a tcl::command_group for the given name,
      then the new tcl::command will be hooked into that
      tcl::command_group as an overload. Otherwise, a brand new
      tcl::command_group will be created. */
  static void make(tcl::interpreter& interp,
                   std::function<void(tcl::call_context&)>&& callback,
                   const char* cmd_name,
                   const char* usage,
                   const tcl::arg_spec& spec,
                   const rutz::file_pos& src_pos,
                   std::unique_ptr<tcl::arg_dispatcher> dispatcher = std::unique_ptr<tcl::arg_dispatcher>());

  /// Add the given tcl::command to this group's overload list.
  void add(tcl::command&& p);

  /// Get this group's fully namespace-qualified command name.
  rutz::fstring resolved_name() const;

  /// Returns a string giving the command's proper usage, including overloads.
  rutz::fstring usage() const;

  int invoke_raw(int s_objc, Tcl_Obj *const objv[]) noexcept;

private:
  class impl;
  friend class impl;
  impl* const rep;

  /// Private constructor; clients shouldn't manipulate tcl::command_group objects directly.
  command_group(tcl::interpreter& interp,
               const rutz::fstring& cmd_name,
               const rutz::file_pos& src_pos);

  /// Private destructor since destruction is automated by Tcl.
  ~command_group() noexcept;

  command_group(const command_group&); // not implemented
  command_group& operator=(const command_group&); // not implemented
};

#endif // !GROOVX_TCL_COMMANDGROUP_H_UTC20050628162421_DEFINED
