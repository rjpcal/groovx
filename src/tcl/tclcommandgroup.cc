///////////////////////////////////////////////////////////////////////
//
// tclcommandgroup.cc
//
// Copyright (c) 2004-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Wed Jun  9 09:45:36 2004
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

#ifndef TCLCOMMANDGROUP_CC_DEFINED
#define TCLCOMMANDGROUP_CC_DEFINED

#include "tcl/tclcommandgroup.h"

#include "tcl/tclcmd.h"
#include "tcl/tclobjptr.h"
#include "tcl/tclsafeinterp.h"

#include "util/pointers.h"
#include "util/strings.h"

#include <list>
#include <tcl.h>

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER

namespace
{
  fstring getFullCommandName(Tcl::Interp& interp, Tcl_Command token)
  {
    Tcl::ObjPtr result;
    // Note, this Tcl API call requires Tcl 8.4.6 or greater (or 8.5
    // or greater)
    Tcl_GetCommandFullName(interp.intp(), token, result.obj());
    return fstring(result.as<const char*>());
  }
}

class Tcl::CommandGroup::Impl
{
public:
  Impl(Tcl::Interp& intp, const fstring& cmd_name,
       const char* src_file_name, int src_line_no) :
    interp(intp),
    cmdToken(Tcl_CreateObjCommand(interp.intp(),
                                  cmd_name.c_str(),
                                  // see comment in CommandGroup's
                                  // constructor for why we pass zeros
                                  // here
                                  static_cast<Tcl_ObjCmdProc*>(0),
                                  static_cast<ClientData>(0),
                                  static_cast<Tcl_CmdDeleteProc*>(0))),
    initialCmdName(getFullCommandName(intp, cmdToken)),
    cmdList(),
    profName("tcl/", cmd_name),
    prof(profName.c_str(), src_file_name, src_line_no)
  {}

  ~Impl() throw() {}

  typedef std::list<shared_ptr<Tcl::Command> > List;

  Tcl::Interp interp;
  const Tcl_Command cmdToken;
  const fstring initialCmdName;
  List cmdList;
  const fstring profName;
  Util::Prof prof;

  fstring usageWarning(const fstring& argv0) const;

private:
  Impl(const Impl&);
  Impl& operator=(const Impl&);
};

namespace
{
  void appendUsage(fstring& dest, const fstring& usage)
  {
    if (!usage.is_empty())
      dest.append(" ", usage);
  }
}

fstring Tcl::CommandGroup::Impl::usageWarning(const fstring& argv0) const
{
DOTRACE("Tcl::CommandGroup::usageWarning");
  fstring warning("wrong # args: should be ");

  if (cmdList.size() == 1)
    {
      warning.append("\"", argv0);
      appendUsage(warning, cmdList.front()->usageString());
      warning.append("\"");
    }
  else
    {
      warning.append("one of:");
      for (Impl::List::const_iterator
             itr = cmdList.begin(),
             end = cmdList.end();
           itr != end;
           ++itr)
        {
          warning.append("\n\t\"", argv0);
          appendUsage(warning, (*itr)->usageString());
          warning.append("\"");
        }
    }

  if (argv0 != initialCmdName)
    warning.append("\n(resolves to ", initialCmdName, ")");

  return warning;
}

Tcl::CommandGroup::CommandGroup(Tcl::Interp& interp,
                                const fstring& cmd_name,
                                const char* src_file_name,
                                int src_line_no) :
  rep(new Impl(interp, cmd_name, src_file_name, src_line_no))
{
DOTRACE("Tcl::CommandGroup::CommandGroup");

  // Register the command procedure. We do a two-step
  // initialization. When we call Tcl_CreateObjCommand in Impl's
  // constructor, we don't fill in the clientData/objProc/deleteProc
  // values there, but instead wait to fill them in here. The reason
  // is that we don't want to set up any callbacks from Tcl until
  // after we're sure that everything else in the construction
  // sequence has succeeded. We want to ensure that we don't have
  // "dangling callbacks" in case an exception escapes from an earlier
  // part of Impl's or CommandGroups's constructor.
  Tcl_CmdInfo info;
  const int result = Tcl_GetCommandInfoFromToken(rep->cmdToken, &info);
  Assert(result == 1);
  Assert(info.isNativeObjectProc == 1);
  info.objClientData = static_cast<ClientData>(this);
  info.objProc = &cInvokeCallback;
  info.deleteData = static_cast<ClientData>(this);
  info.deleteProc = &cDeleteCallback;
  Tcl_SetCommandInfoFromToken(rep->cmdToken, &info);

  Tcl_CreateExitHandler(&cExitCallback,
                        static_cast<ClientData>(this));
}

// A destruction sequence can get triggered in a number of ways:
/*
   (1) application exit might trigger the cExitCallback

   (2) the cDeleteCallback might get triggered either by explicit
       deletion by the user (e.g. [rename]ing the command to the empty
       string "")

   General principles:

   (1) it is always "safe" to destroy the Tcl_Command, in the sense
       that it can't cause any crashes... in particular, it's OK to
       destroy the Tcl_Command even if rep->cmdList is not empty; that
       would just mean that the remaining Tcl::Command objects in
       rep->cmdList won't have any input sent their way
 */
Tcl::CommandGroup::~CommandGroup() throw()
{
DOTRACE("Tcl::CommandGroup::~CommandGroup");

  Tcl_DeleteExitHandler(&cExitCallback,
                        static_cast<ClientData>(this));

  delete rep;
}

Tcl::CommandGroup* Tcl::CommandGroup::lookup(Tcl::Interp& interp,
                                             const char* name) throw()
{
DOTRACE("Tcl::CommandGroup::lookup");
  /*
     typedef struct Tcl_CmdInfo {
       int isNativeObjectProc;
       Tcl_ObjCmdProc *objProc;
       ClientData objClientData;
       Tcl_CmdProc *proc;
       ClientData clientData;
       Tcl_CmdDeleteProc *deleteProc;
       ClientData deleteData;
       Tcl_Namespace *namespacePtr;
     } Tcl_CmdInfo;
  */
  Tcl_CmdInfo info;
  const int result = Tcl_GetCommandInfo(interp.intp(), name, &info);
  if (result == 1 &&
      info.isNativeObjectProc == 1 &&
      info.objProc == cInvokeCallback &&
      info.deleteProc == cDeleteCallback)
    {
      return static_cast<CommandGroup*>(info.objClientData);
    }
  return 0;
}

Tcl::CommandGroup* Tcl::CommandGroup::make(Tcl::Interp& interp,
                                           const fstring& cmd_name,
                                           const char* src_file_name,
                                           int src_line_no)
{
DOTRACE("Tcl::CommandGroup::make");
  CommandGroup* const c =
    Tcl::CommandGroup::lookup(interp, cmd_name.c_str());

  if (c != 0)
    return c;

  // else...
  return new CommandGroup(interp, cmd_name, src_file_name, src_line_no);
}

void Tcl::CommandGroup::add(shared_ptr<Tcl::Command> p)
{
DOTRACE("Tcl::CommandGroup::add");
  rep->cmdList.push_back(p);
}

fstring Tcl::CommandGroup::cmdName() const
{
  return getFullCommandName(rep->interp, rep->cmdToken);
}

fstring Tcl::CommandGroup::usage() const
{
DOTRACE("Tcl::CommandGroup::usage");
  fstring result;

  Impl::List::const_iterator
    itr = rep->cmdList.begin(),
    end = rep->cmdList.end();

  while (true)
    {
      result.append("\t", cmdName());
      appendUsage(result, (*itr)->usageString());
      if (++itr != end)
        result.append("\n");
      else
        break;
    }

  return result;
}

int Tcl::CommandGroup::rawInvoke(int s_objc, Tcl_Obj *const objv[]) throw()
{
DOTRACE("Tcl::CommandGroup::rawInvoke");

  // This is to use the separate Util::Prof object that each
  // CommandGroup has. This way we can trace the timing of individual
  // Tcl commands.
  Util::Trace tracer(rep->prof, DYNAMIC_TRACE_EXPR);

  // Should always be at least one since there is always the
  // command-name itself as the first argument.
  Assert(s_objc >= 1);

  if (GET_DBG_LEVEL() > 1)
    {
      for (int argi = 0; argi < s_objc; ++argi)
        {
          const char* arg = Tcl_GetString(objv[argi]);
          dbgPrint(1, argi);
          dbgPrint(1, " argv = ");
          dbgPrintNL(1, arg);
        }
    }

  unsigned int objc = (unsigned int) s_objc;

  // catch all possible exceptions since this is a callback from C
  try
    {
      for (Impl::List::const_iterator
             itr = rep->cmdList.begin(),
             end = rep->cmdList.end();
           itr != end;
           ++itr)
        {
          if ((*itr)->rejectsObjc(objc))
            continue;

          // Found a matching overload, so try it:
          (*itr)->call(rep->interp, objc, objv);

          if (GET_DBG_LEVEL() > 1)
            {
              const char* result = rep->interp.getResult<const char*>();
              dbgEvalNL(1, result);
            }
          return TCL_OK;
        }

      const fstring argv0(Tcl_GetString(objv[0]));

      // Here, we run out of potential overloads, so abort the command.
      rep->interp.resetResult();
      rep->interp.appendResult(rep->usageWarning(argv0).c_str());
      return TCL_ERROR;
    }
  catch (...)
    {
      rep->interp.handleLiveException(Tcl_GetString(objv[0]), false);
    }

  return TCL_ERROR;
}

int Tcl::CommandGroup::cInvokeCallback(ClientData clientData,
                                       Tcl_Interp* interp,
                                       int s_objc,
                                       Tcl_Obj *const objv[]) throw()
{
  CommandGroup* c = static_cast<CommandGroup*>(clientData);

  Assert(c != 0);
  Assert(interp == c->rep->interp.intp());

  return c->rawInvoke(s_objc, objv);
}

void Tcl::CommandGroup::cDeleteCallback(ClientData clientData) throw()
{
DOTRACE("Tcl::CommandGroup::cDeleteCallback");
  CommandGroup* c = static_cast<CommandGroup*>(clientData);
  Assert(c != 0);
  delete c;
}

void Tcl::CommandGroup::cExitCallback(ClientData clientData) throw()
{
DOTRACE("Tcl::CommandGroup::cExitCallback");
  CommandGroup* c = static_cast<CommandGroup*>(clientData);
  Assert(c != 0);
  Tcl_DeleteCommandFromToken(c->rep->interp.intp(), c->rep->cmdToken);
}

static const char vcid_tclcommandgroup_cc[] = "$Header$";
#endif // !TCLCOMMANDGROUP_CC_DEFINED
