///////////////////////////////////////////////////////////////////////
//
// tclcmd.cc
//
// Copyright (c) 1999-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Fri Jun 11 14:50:58 1999
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

#ifndef TCLCMD_CC_DEFINED
#define TCLCMD_CC_DEFINED

#include "tcl/tclcmd.h"

#include "tcl/tclsafeinterp.h"

#include "util/strings.h"

#include <list>
#include <map>
#include <tcl.h>

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER

///////////////////////////////////////////////////////////////////////
//
// File scope definitions
//
///////////////////////////////////////////////////////////////////////

namespace
{
  class Overloads;

  // Holds the the addresses of all valid Overload objects (this is
  // managed in Overload's constructor+destructor)
  typedef std::map<fstring, Overloads*> CmdTable;

  CmdTable* commandTable_ = 0;

  CmdTable& commandTable()
  {
    if (commandTable_ == 0)
      commandTable_ = new CmdTable;
    return *commandTable_;
  }

  class Overloads
  {
  private:
    typedef std::list<Tcl::Command*> List;

    Tcl::Interp itsInterp;
    const fstring itsCmdName;
    Tcl_Command itsCmdToken;
    List itsList;
    int itsRefCount;
    const fstring itsProfName;
    Util::Prof itsProf;

    Overloads(Tcl::Interp& interp, const fstring& cmd_name);
    ~Overloads() throw();

    Overloads(const Overloads&); // not implemented
    Overloads& operator=(const Overloads&); // not implemented

    static int cInvokeCallback(ClientData clientData,
                               Tcl_Interp*, /* use Overloads's own Tcl::Interp */
                               int s_objc,
                               Tcl_Obj *const objv[]) throw()
    {
      Overloads* ov = static_cast<Overloads*>(clientData);

      Assert(ov != 0);

      return ov->rawInvoke(s_objc, objv);
    }

    static void cDeleteCallback(ClientData clientData)
    {
    DOTRACE("Overloads::cDeleteCallback");
      Overloads* ov = static_cast<Overloads*>(clientData);
      Assert(ov != 0);
      ov->itsCmdToken = 0; // since this callback is notifying us that the
                           // command was deleted
      if (ov->itsList.empty())
        delete ov;
    }

    static void cExitCallback(ClientData clientData)
    {
    DOTRACE("Overloads::cExitCallback");
      Overloads* ov = static_cast<Overloads*>(clientData);
      Assert(ov != 0);
      if (ov->itsCmdToken != 0)
        Tcl_DeleteCommandFromToken(ov->itsInterp.intp(), ov->itsCmdToken);
    }

  public:
    static Overloads* make(Tcl::Interp& interp, const fstring& cmd_name);

    void add(Tcl::Command* p)
    {
    DOTRACE("Overloads::add");
      itsList.push_back(p);
    }

    void remove(Tcl::Command* p)
    {
    DOTRACE("Overloads::remove");
      itsList.remove(p);
      // itsList is used as an implicit reference-count... each time a
      // Tcl::Command is created, it creates a reference by calling add()
      // on its Overloads object, and when the Tcl::Command is destroyed,
      // it calls remove() on its Overloads object. So, when the Overloads
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

    Tcl::Command* first() const { return itsList.front(); }

    const fstring& cmdName() const { return itsCmdName; }

    fstring usage() const;

    fstring usageWarning() const;

    int rawInvoke(int s_objc, Tcl_Obj *const objv[]) throw();
  };
}

Overloads::Overloads(Tcl::Interp& interp, const fstring& cmd_name) :
  itsInterp(interp),
  itsCmdName(cmd_name),
  itsCmdToken(0),
  itsList(),
  itsRefCount(0),
  itsProfName("tcl/", cmd_name),
  itsProf(itsProfName.c_str())
{
DOTRACE("Overloads::Overloads");

  // Register the command procedure
  itsCmdToken =
    Tcl_CreateObjCommand(interp.intp(),
                         cmd_name.c_str(),
                         &cInvokeCallback,
                         static_cast<ClientData>(this),
                         &cDeleteCallback);

  Tcl_CreateExitHandler(&cExitCallback,
                        static_cast<ClientData>(this));

  ++itsRefCount;

  // Make sure there isn't already an Overloads object for this cmdname
  CmdTable::iterator pos = commandTable().find(itsCmdName);
  Assert( pos == commandTable().end() );
  // Register this Overloads object with the given cmdname
  commandTable().insert(CmdTable::value_type(itsCmdName, this));
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
       Tcl_Command even if Overloads::itsList is not empty; that would just
       mean that the remaining Tcl::Command objects in itsList won't have
       any input sent their way

   (2) however, if possible we want to wait to destroy the Tcl_Command
       object until itsList is empty
 */
Overloads::~Overloads() throw()
{
DOTRACE("Overloads::~Overloads");

  Assert( itsList.empty() );
  Assert( itsCmdToken == 0 );

  CmdTable::iterator pos = commandTable().find(cmdName());
  Assert( pos != commandTable().end() );
  Assert( (*pos).second == this );
  commandTable().erase(pos);

  Tcl_DeleteExitHandler(&cExitCallback,
                        static_cast<ClientData>(this));
}

Overloads* Overloads::make(Tcl::Interp& interp, const fstring& cmd_name)
{
DOTRACE("Overloads::make");
  CmdTable::iterator pos = commandTable().find(cmd_name);
  if (pos != commandTable().end())
    {
      return (*pos).second;
    }
  else
    {
      return new Overloads(interp, cmd_name);
    }
}

fstring Overloads::usage() const
{
DOTRACE("Overloads::usage");
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

fstring Overloads::usageWarning() const
{
DOTRACE("Overloads::usageWarning");
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

int Overloads::rawInvoke(int s_objc, Tcl_Obj *const objv[]) throw()
{
DOTRACE("Overloads::rawInvoke");

  // This is to use the separate Util::Prof object that each Overloads
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
      for (Overloads::List::const_iterator
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

///////////////////////////////////////////////////////////////////////
//
// Tcl::Dispatcher
//
///////////////////////////////////////////////////////////////////////

Tcl::Dispatcher::~Dispatcher() throw() {}

///////////////////////////////////////////////////////////////////////
//
// DefaultDispatcher
//
///////////////////////////////////////////////////////////////////////

namespace
{
  class DefaultDispatcher : public Tcl::Dispatcher
  {
  public:
    virtual void dispatch(Tcl::Interp& interp,
                          unsigned int objc, Tcl_Obj* const objv[],
                          Tcl::Command& cmd)
    {
      Tcl::Context ctx(interp, objc, objv);
      cmd.invoke(ctx);
    }
  };

  shared_ptr<DefaultDispatcher>
    theDefaultDispatcher(new DefaultDispatcher);
}

///////////////////////////////////////////////////////////////////////
//
// Tcl::Command::Impl class definition
//
///////////////////////////////////////////////////////////////////////

class Tcl::Command::Impl
{
private:
  Impl(const Impl&);
  Impl& operator=(const Impl&);

public:
  Impl(const char* cmd_name, const char* usg,
       int objc_min, int objc_max, bool exact_objc) :
    dispatcher(theDefaultDispatcher),
    usage(usg ? usg : ""),
    objcMin(objc_min < 0 ? 0 : (unsigned int) objc_min),
    objcMax( (objc_max > 0) ? (unsigned int) objc_max : objcMin),
    exactObjc(exact_objc),
    overloads(0)
  {}

  ~Impl() throw() {}

  // These are set once per command object
  shared_ptr<Tcl::Dispatcher> dispatcher;
  const fstring usage;
  const unsigned int objcMin;
  const unsigned int objcMax;
  const bool exactObjc;
  Overloads* overloads;
};

///////////////////////////////////////////////////////////////////////
//
// Tcl::Command member definitions
//
///////////////////////////////////////////////////////////////////////

Tcl::Command::Command(Tcl::Interp& interp,
                      const char* cmd_name, const char* usage,
                      int objc_min, int objc_max, bool exact_objc)
  :
  rep(new Impl(cmd_name, usage, objc_min, objc_max, exact_objc))
{
DOTRACE("Tcl::Command::Command");

  rep->overloads = Overloads::make(interp, cmd_name);

  Assert(rep->overloads != 0);

  rep->overloads->add(this);
}

Tcl::Command::~Command() throw()
{
DOTRACE("Tcl::Command::~Command");

  rep->overloads->remove(this);

  rep->overloads = 0;

  delete rep;
}

const fstring& Tcl::Command::name() const
{
DOTRACE("Tcl::Command::name");
  return rep->overloads->cmdName();
}

fstring Tcl::Command::usage() const
{
DOTRACE("Tcl::Command::usage");
  return rep->overloads->usage();
}

bool Tcl::Command::allowsObjc(unsigned int objc) const
{
DOTRACE("Tcl::Command::allowsObjc");
  if (rep->exactObjc)
    {
      return (objc == rep->objcMin || objc == rep->objcMax);
    }

  return (objc >= rep->objcMin && objc <= rep->objcMax);
}

bool Tcl::Command::rejectsObjc(unsigned int objc) const
{
  return !allowsObjc(objc);
}

fstring Tcl::Command::rawUsage() const
{
DOTRACE("Tcl::Command::rawUsage");
  fstring result(rep->overloads->cmdName());
  if (!rep->usage.is_empty())
    result.append(" ", rep->usage);
  return result;
}

shared_ptr<Tcl::Dispatcher> Tcl::Command::getDispatcher() const
{
DOTRACE("Tcl::Command::getDispatcher");
  return rep->dispatcher;
}

void Tcl::Command::setDispatcher(shared_ptr<Tcl::Dispatcher> dpx)
{
DOTRACE("Tcl::Command::setDispatcher");
  rep->dispatcher = dpx;
}

Tcl::Command* Tcl::Command::lookup(Tcl::Interp&, const char* name)
{
DOTRACE("Tcl::Command::lookup");
  Overloads* const ov = commandTable()[name];

  if (ov == 0)
    return 0;

  // else...
  return ov->first();
}


///////////////////////////////////////////////////////////////////////
//
// Tcl::Context member definitions
//
///////////////////////////////////////////////////////////////////////

Tcl::Context::Context(Tcl::Interp& interp,
                      unsigned int objc, Tcl_Obj* const* objv) :
  itsInterp(interp),
  itsObjc(objc),
  itsObjv(objv)
{}

Tcl::Context::~Context() throw()
{}

void Tcl::Context::setObjResult(const Tcl::ObjPtr& obj)
{
DOTRACE("Tcl::Context::setObjResult");
  itsInterp.setResult(obj);
}

static const char vcid_tclcmd_cc[] = "$Header$";
#endif // !TCLCMD_CC_DEFINED
