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

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER

Tcl::CommandGroup::CommandGroup(Tcl::Interp& interp, const fstring& cmd_name) :
  itsInterp(interp),
  itsCmdName(cmd_name),
  itsCmdToken(0),
  itsList(),
  itsProfName("tcl/", cmd_name),
  itsProf(itsProfName.c_str())
{
DOTRACE("Tcl::CommandGroup::CommandGroup");

  // Register the command procedure
  itsCmdToken =
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
   (1) remove() might be called enough times that itsList becomes empty
   (2) application exit might trigger the cExitCallback

   (3) the cDeleteCallback might get triggered either by explicit deletion
       by the user (e.g. [rename]ing the command to the empty string "")

   General principles:

   (1) it is always "safe" to destroy the Tcl_Command, in the sense that it
       can't cause any crashes... in particular, it's OK to destroy the
       Tcl_Command even if CommandGroup::itsList is not empty; that would just
       mean that the remaining Tcl::Command objects in itsList won't have
       any input sent their way

   (2) however, if possible we want to wait to destroy the Tcl_Command
       object until itsList is empty
 */
Tcl::CommandGroup::~CommandGroup() throw()
{
DOTRACE("Tcl::CommandGroup::~CommandGroup");

  Assert( itsList.empty() );
  Assert( itsCmdToken == 0 );

  Tcl_DeleteExitHandler(&cExitCallback,
                        static_cast<ClientData>(this));
}

Tcl::CommandGroup* Tcl::CommandGroup::lookup(Tcl::Interp& interp,
                                             const char* name)
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

void Tcl::CommandGroup::add(Tcl::Command* p)
{
DOTRACE("Tcl::CommandGroup::add");
  itsList.push_back(p);
}

void Tcl::CommandGroup::remove(Tcl::Command* p)
{
DOTRACE("Tcl::CommandGroup::remove");
  itsList.remove(p);
  // itsList is used as an implicit reference-count... each time a
  // Tcl::Command is created, it creates a reference by calling add()
  // on its CommandGroup object, and when the Tcl::Command is destroyed,
  // it calls remove() on its CommandGroup object. So, when the CommandGroup
  // itsList becomes empty, it is no longer referenced by any
  // Tcl::Command objects and can thus be deleted.
  if (itsList.empty())
    {
      if (itsCmdToken != 0)
        {
          Tcl_DeleteCommandFromToken(itsInterp.intp(), itsCmdToken);
        }
      else
        {
          delete this;
        }
    }
}

fstring Tcl::CommandGroup::usage() const
{
DOTRACE("Tcl::CommandGroup::usage");
  fstring result;

  List::const_iterator
    itr = itsList.begin(),
    end = itsList.end();

  while (true)
    {
      result.append("\t", (*itr)->rawUsage());
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

  if (itsList.size() == 1)
    {
      warning.append("\"", itsList.front()->rawUsage(), "\"");
    }
  else
    {
      warning.append("one of:");
      for (List::const_iterator
             itr = itsList.begin(),
             end = itsList.end();
           itr != end;
           ++itr)
        {
          warning.append("\n\t\"", (*itr)->rawUsage(), "\"");
        }
    }

  return warning;
}

int Tcl::CommandGroup::rawInvoke(int s_objc, Tcl_Obj *const objv[]) throw()
{
DOTRACE("Tcl::CommandGroup::rawInvoke");

  // This is to use the separate Util::Prof object that each CommandGroup
  // has. This way we can trace the timing of individual Tcl commands.
  Util::Trace tracer(itsProf, DYNAMIC_TRACE_EXPR);

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
      for (List::const_iterator
             itr = itsList.begin(),
             end = itsList.end();
           itr != end;
           ++itr)
        {
          if ((*itr)->rejectsObjc(objc))
            continue;

          // Found a matching overload, so try it:
          (*itr)->getDispatcher()->dispatch(itsInterp, objc, objv, **itr);

          if (GET_DBG_LEVEL() > 1)
            {
              const char* result = itsInterp.getResult<const char*>();
              dbgEvalNL(1, result);
            }
          return TCL_OK;
        }

      // Here, we run out of potential overloads, so abort the command.
      itsInterp.resetResult();
      itsInterp.appendResult(usageWarning().c_str());
      return TCL_ERROR;
    }
  catch (...)
    {
      itsInterp.handleLiveException(itsCmdName.c_str(), false);
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
  Assert(interp == c->itsInterp.intp());

  return c->rawInvoke(s_objc, objv);
}

void Tcl::CommandGroup::cDeleteCallback(ClientData clientData) throw()
{
DOTRACE("Tcl::CommandGroup::cDeleteCallback");
  CommandGroup* c = static_cast<CommandGroup*>(clientData);
  Assert(c != 0);
  c->itsCmdToken = 0; // since this callback is notifying us that the
                      // command was deleted
  if (c->itsList.empty())
    delete c;
}

void Tcl::CommandGroup::cExitCallback(ClientData clientData) throw()
{
DOTRACE("Tcl::CommandGroup::cExitCallback");
  CommandGroup* c = static_cast<CommandGroup*>(clientData);
  Assert(c != 0);
  if (c->itsCmdToken != 0)
    Tcl_DeleteCommandFromToken(c->itsInterp.intp(), c->itsCmdToken);
}

static const char vcid_tclcommandgroup_cc[] = "$Header$";
#endif // !TCLCOMMANDGROUP_CC_DEFINED
