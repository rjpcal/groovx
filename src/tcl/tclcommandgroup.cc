///////////////////////////////////////////////////////////////////////
//
// tclcommandgroup.cc
//
// Copyright (c) 2004-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Wed Jun  9 09:45:36 2004
// commit: $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLCOMMANDGROUP_CC_DEFINED
#define TCLCOMMANDGROUP_CC_DEFINED

#include "tcl/tclcommandgroup.h"

#include "tcl/tclcmd.h"
#include "tcl/tclsafeinterp.h"

#include "util/pointers.h"
#include "util/strings.h"

#include <list>
#include <tcl.h>

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER

class Tcl::CommandGroup::Impl
{
public:
  Impl(Tcl::Interp& intp, const fstring& cmd_name) :
    interp(intp),
    cmdName(cmd_name),
    cmdToken(0),
    cmdList(),
    profName("tcl/", cmd_name),
    prof(profName.c_str())
  {}

  ~Impl() throw() {}

  typedef std::list<shared_ptr<Tcl::Command> > List;

  Tcl::Interp interp;
  const fstring cmdName;
  Tcl_Command cmdToken;
  List cmdList;
  const fstring profName;
  Util::Prof prof;

private:
  Impl(const Impl&);
  Impl& operator=(const Impl&);
};

Tcl::CommandGroup::CommandGroup(Tcl::Interp& interp, const fstring& cmd_name) :
  rep(new Impl(interp, cmd_name))
{
DOTRACE("Tcl::CommandGroup::CommandGroup");

  // Register the command procedure
  rep->cmdToken =
    Tcl_CreateObjCommand(interp.intp(),
                         cmd_name.c_str(),
                         &cInvokeCallback,
                         static_cast<ClientData>(this),
                         &cDeleteCallback);

  Tcl_CreateExitHandler(&cExitCallback,
                        static_cast<ClientData>(this));
}

// A destruction sequence can get triggered in a number of ways:
/*
   (1) application exit might trigger the cExitCallback

   (2) the cDeleteCallback might get triggered either by explicit deletion
       by the user (e.g. [rename]ing the command to the empty string "")

   General principles:

   (1) it is always "safe" to destroy the Tcl_Command, in the sense that it
       can't cause any crashes... in particular, it's OK to destroy the
       Tcl_Command even if rep->cmdList is not empty; that would just mean that
       the remaining Tcl::Command objects in rep->cmdList won't have any input
       sent their way
 */
Tcl::CommandGroup::~CommandGroup() throw()
{
DOTRACE("Tcl::CommandGroup::~CommandGroup");

  Assert( rep->cmdToken == 0 );

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
                                           const fstring& cmd_name)
{
DOTRACE("Tcl::CommandGroup::make");
  CommandGroup* const c = Tcl::CommandGroup::lookup(interp, cmd_name.c_str());

  if (c != 0)
    return c;

  // else...
  return new CommandGroup(interp, cmd_name);
}

void Tcl::CommandGroup::add(shared_ptr<Tcl::Command> p)
{
DOTRACE("Tcl::CommandGroup::add");
  rep->cmdList.push_back(p);
}

const fstring& Tcl::CommandGroup::cmdName() const
{
  return rep->cmdName;
}

namespace
{
  void appendUsage(fstring& dest, const fstring& usage)
  {
    if (!usage.is_empty())
      dest.append(" ", usage);
  }
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
      result.append("\t", rep->cmdName);
      appendUsage(result, (*itr)->usageString());
      if (++itr != end)
        result.append("\n");
      else
        break;
    }

  return result;
}

fstring Tcl::CommandGroup::usageWarning() const
{
DOTRACE("Tcl::CommandGroup::usageWarning");
  fstring warning("wrong # args: should be ");

  if (rep->cmdList.size() == 1)
    {
      warning.append("\"", rep->cmdName);
      appendUsage(warning, rep->cmdList.front()->usageString());
      warning.append("\"");
    }
  else
    {
      warning.append("one of:");
      for (Impl::List::const_iterator
             itr = rep->cmdList.begin(),
             end = rep->cmdList.end();
           itr != end;
           ++itr)
        {
          warning.append("\n\t\"", rep->cmdName);
          appendUsage(warning, (*itr)->usageString());
          warning.append("\"");
        }
    }

  return warning;
}

int Tcl::CommandGroup::rawInvoke(int s_objc, Tcl_Obj *const objv[]) throw()
{
DOTRACE("Tcl::CommandGroup::rawInvoke");

  // This is to use the separate Util::Prof object that each CommandGroup
  // has. This way we can trace the timing of individual Tcl commands.
  Util::Trace tracer(rep->prof, DYNAMIC_TRACE_EXPR);

  Assert(s_objc >= 0);

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

      // Here, we run out of potential overloads, so abort the command.
      rep->interp.resetResult();
      rep->interp.appendResult(usageWarning().c_str());
      return TCL_ERROR;
    }
  catch (...)
    {
      rep->interp.handleLiveException(rep->cmdName.c_str(), false);
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
  c->rep->cmdToken = 0; // since this callback is notifying us that the
                        // command was deleted
  // FIXME need to notify cmdList that we're being deleted?
  delete c;
}

void Tcl::CommandGroup::cExitCallback(ClientData clientData) throw()
{
DOTRACE("Tcl::CommandGroup::cExitCallback");
  CommandGroup* c = static_cast<CommandGroup*>(clientData);
  Assert(c != 0);
  if (c->rep->cmdToken != 0)
    Tcl_DeleteCommandFromToken(c->rep->interp.intp(), c->rep->cmdToken);
}

static const char vcid_tclcommandgroup_cc[] = "$Header$";
#endif // !TCLCOMMANDGROUP_CC_DEFINED
