///////////////////////////////////////////////////////////////////////
//
// tclcommandgroup.h
//
// Copyright (c) 2004-2004
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

class fstring;

struct FilePosition;

template <class T> class shared_ptr;

namespace Tcl
{
  class Command;
  class CommandGroup;
  class Interp;
}

class Tcl::CommandGroup
{
public:
  /// Find the command associated with a given name.
  /** Returns null if no such command. FIXME Fix this so that it works
      with inherited commands. */
  static CommandGroup* lookup(Tcl::Interp& interp,
                              const char* name) throw();

  /// Find the command for the given name, making a new one if necessary.
  static CommandGroup* make(Tcl::Interp& interp,
                            const fstring& cmd_name,
                            const FilePosition& src_pos);

  void add(shared_ptr<Tcl::Command> p);

  fstring cmdName() const;

  /// Returns a string giving the command's proper usage, including overloads.
  fstring usage() const;

  int rawInvoke(int s_objc, Tcl_Obj *const objv[]) throw();

private:
  class Impl;
  Impl* const rep;

  CommandGroup(Tcl::Interp& interp, const fstring& cmd_name,
               const FilePosition& src_pos);
  ~CommandGroup() throw();

  CommandGroup(const CommandGroup&); // not implemented
  CommandGroup& operator=(const CommandGroup&); // not implemented

  static int cInvokeCallback(ClientData clientData,
                             Tcl_Interp* interp,
                             int s_objc,
                             Tcl_Obj *const objv[]) throw();

  static void cDeleteCallback(ClientData clientData) throw();

  static void cExitCallback(ClientData clientData) throw();
};

static const char vcid_tclcommandgroup_h[] = "$Header$";
#endif // !TCLCOMMANDGROUP_H_DEFINED
