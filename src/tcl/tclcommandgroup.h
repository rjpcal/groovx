///////////////////////////////////////////////////////////////////////
//
// tclcommandgroup.h
//
// Copyright (c) 2004-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Wed Jun  9 09:45:26 2004
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

#ifndef TCLCOMMANDGROUP_H_DEFINED
#define TCLCOMMANDGROUP_H_DEFINED

#include "tcl/tclsafeinterp.h"

#include "util/strings.h"

#include "util/trace.h"

#include <list>
#include <tcl.h>

namespace Tcl
{
  class Command;
  class CommandGroup;
}

class Tcl::CommandGroup
{
public:
  static CommandGroup* lookup(Tcl::Interp& interp, const char* name);

  static CommandGroup* make(Tcl::Interp& interp, const fstring& cmd_name);

  void add(Tcl::Command* p);

  void remove(Tcl::Command* p);

  Tcl::Command* first() const { return itsList.front(); }

  const fstring& cmdName() const { return itsCmdName; }

  fstring usage() const;

  fstring usageWarning() const;

  int rawInvoke(int s_objc, Tcl_Obj *const objv[]) throw();

private:
  typedef std::list<Tcl::Command*> List;

  Tcl::Interp itsInterp;
  const fstring itsCmdName;
  Tcl_Command itsCmdToken;
  List itsList;
  const fstring itsProfName;
  Util::Prof itsProf;

  CommandGroup(Tcl::Interp& interp, const fstring& cmd_name);
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
