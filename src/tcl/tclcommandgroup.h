///////////////////////////////////////////////////////////////////////
//
// tclcommandgroup.h
//
// Copyright (c) 2004-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Wed Jun  9 09:45:26 2004
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

#ifndef TCLCOMMANDGROUP_H_DEFINED
#define TCLCOMMANDGROUP_H_DEFINED

typedef struct Tcl_Obj Tcl_Obj;
struct Tcl_Interp;
typedef void* ClientData;

namespace rutz
{
  class file_pos;
  class fstring;
  template <class T> class shared_ptr;
}

namespace Tcl
{
  class Command;
  class CommandGroup;
  class Interp;
}

/// Represents a set of overloaded Tcl::Command objects.
class Tcl::CommandGroup
{
public:
  /// Find the named command, if it exists.
  /** Returns null if no such command. */
  static CommandGroup* lookup(Tcl::Interp& interp,
                              const char* name) throw();

  /// Find the named command, after following any namespace aliases.
  /** Returns null if no such command. */
  static CommandGroup* lookupOriginal(Tcl::Interp& interp,
                                      const char* name) throw();

  /// Find the named command, making a new one if necessary.
  static CommandGroup* make(Tcl::Interp& interp,
                            const rutz::fstring& cmd_name,
                            const rutz::file_pos& src_pos);

  /// Add the given Tcl::Command to this group's overload list.
  void add(rutz::shared_ptr<Tcl::Command> p);

  /// Get this group's fully namespace-qualified command name.
  rutz::fstring cmdName() const;

  /// Returns a string giving the command's proper usage, including overloads.
  rutz::fstring usage() const;

  int rawInvoke(int s_objc, Tcl_Obj *const objv[]) throw();

private:
  class Impl;
  friend class Impl;
  Impl* const rep;

  /// Private constructor since clients should use CommandGroup::make().
  CommandGroup(Tcl::Interp& interp,
               const rutz::fstring& cmd_name,
               const rutz::file_pos& src_pos);

  /// Private destructor since destruction is automated by Tcl.
  ~CommandGroup() throw();

  CommandGroup(const CommandGroup&); // not implemented
  CommandGroup& operator=(const CommandGroup&); // not implemented
};

static const char vcid_tclcommandgroup_h[] = "$Id$ $URL$";
#endif // !TCLCOMMANDGROUP_H_DEFINED
