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

#define TRACE_USE_COUNT

#ifdef TRACE_USE_COUNT
#  include <fstream>
#endif

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
  class Overloads
  {
  public:
    typedef std::list<Tcl::Command*> List;

    Tcl::Interp itsInterp;
    const fstring itsCmdName;
    List itsList;

    Overloads(Tcl::Interp& interp, const char* cmd_name);

    void add(Tcl::Command* p) { itsList.push_back(p); }

    void remove(Tcl::Command* p) { itsList.remove(p); }

    fstring usage() const
    {
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

    fstring usageWarning() const
    {
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

    int rawInvoke(int s_objc, Tcl_Obj *const objv[]) throw();
  };

  // Holds the the addresses of all valid Tcl::Command objects (this is
  // managed in Tcl::Command's constructor+destructor)
  typedef std::map<fstring, Tcl::Command*> CmdTable;

  CmdTable* commandTable_ = 0;

  CmdTable& commandTable()
  {
    if (commandTable_ == 0)
      commandTable_ = new CmdTable;
    return *commandTable_;
  }

#ifdef TRACE_USE_COUNT
  STD_IO::ofstream* USE_COUNT_STREAM = new STD_IO::ofstream("tclprof.out");
#endif
}

namespace
{
  int cInvokeCallback(ClientData clientData,
                      Tcl_Interp*, /* use Tcl::Command's own Tcl::Interp */
                      int s_objc,
                      Tcl_Obj *const objv[]) throw()
  {
    Overloads* ov = static_cast<Overloads*>(clientData);

    Assert(ov != 0);

    return ov->rawInvoke(s_objc, objv);
  }
}

Overloads::Overloads(Tcl::Interp& interp, const char* cmd_name) :
  itsInterp(interp),
  itsCmdName(cmd_name),
  itsList()
{
DOTRACE("Overloads::Overloads");

  Tcl_CreateObjCommand(interp.intp(),
                       cmd_name,
                       cInvokeCallback,
                       static_cast<ClientData>(this),
                       (Tcl_CmdDeleteProc*) NULL);
}

int Overloads::rawInvoke(int s_objc, Tcl_Obj *const objv[]) throw()
{
DOTRACE("Overloads::rawInvoke");

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

  return (*itsList.begin())->rawInvoke(s_objc, objv);
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
  Impl(Tcl::Interp& intp,
       const char* cmd_name, const char* usg,
       int objc_min, int objc_max, bool exact_objc) :
    interp(intp),
    dispatcher(theDefaultDispatcher),
    usage(usg ? usg : ""),
    objcMin(objc_min < 0 ? 0 : (unsigned int) objc_min),
    objcMax( (objc_max > 0) ? (unsigned int) objc_max : objcMin),
    exactObjc(exact_objc),
    cmdName(cmd_name),
    useCount(0),
    overloads()
  {}

  ~Impl() throw() {}

  // These are set once per command object
  Tcl::Interp interp;
  shared_ptr<Tcl::Dispatcher> dispatcher;
  const fstring usage;
  const unsigned int objcMin;
  const unsigned int objcMax;
  const bool exactObjc;
  const fstring cmdName;
  int useCount;
  shared_ptr<Overloads> overloads;
};

///////////////////////////////////////////////////////////////////////
//
// Tcl::Command member definitions
//
///////////////////////////////////////////////////////////////////////

Tcl::Command::Command(Tcl::Interp& interp,
                      const char* cmd_name, const char* usage,
                      int objc_min, int objc_max, bool exact_objc) :
  rep(new Impl(interp, cmd_name, usage,
               objc_min, objc_max, exact_objc))
{
DOTRACE("Tcl::Command::Command");

  // Register the command procedure
  Tcl::Command* previousCmd = commandTable()[rep->cmdName];

  // Only add as an overload if there is an existing previousCmd registered
  // with the same name as us.
  if ( previousCmd != 0 )
    {
      rep->overloads = previousCmd->rep->overloads;
    }
  else
    {
      rep->overloads.reset( new Overloads(interp, cmd_name) );

      commandTable()[rep->cmdName] = this;
    }

  rep->overloads->add(this);
}

Tcl::Command::~Command() throw()
{
DOTRACE("Tcl::Command::~Command");

#ifdef TRACE_USE_COUNT
  if (USE_COUNT_STREAM->good())
    {
      *USE_COUNT_STREAM << rep->cmdName << " "
                        << rep->useCount << STD_IO::endl;
    }
#endif

  rep->overloads->remove(this);

  CmdTable::iterator pos = commandTable().find(rep->cmdName);

  Assert( pos != commandTable().end() );

  if ((*pos).second == this)
    {
      commandTable().erase(pos);
      rep->interp.deleteCommand(rep->cmdName.c_str());
    }

  delete rep;
}

const fstring& Tcl::Command::name() const
{
DOTRACE("Tcl::Command::name");
  return rep->cmdName;
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

int Tcl::Command::rawInvoke(int s_objc, Tcl_Obj* const objv[]) throw()
{
DOTRACE("Tcl::Command::rawInvoke");

  unsigned int objc = (unsigned int) s_objc;

  Tcl::Interp& interp = rep->interp;

  // catch all possible exceptions since this is a callback from C
  try
    {
      ++rep->useCount;

      for (Overloads::List::const_iterator
             itr = rep->overloads->itsList.begin(),
             end = rep->overloads->itsList.end();
           itr != end;
           ++itr)
        {
          if ((*itr)->rejectsObjc(objc))
            continue;

          // Found a matching overload, so try it:
          (*itr)->getDispatcher()->dispatch(interp, objc, objv, **itr);

          if (GET_DBG_LEVEL() > 1)
            {
              const char* result = interp.getResult<const char*>();
              dbgEvalNL(1, result);
            }
          return TCL_OK;
        }

      // Here, we run out of potential overloads, so abort the command.
      interp.resetResult();
      interp.appendResult(rep->overloads->usageWarning().c_str());
      return TCL_ERROR;
    }
  catch (...)
    {
      interp.handleLiveException(rep->cmdName.c_str(), false);
    }

  return TCL_ERROR;
}

fstring Tcl::Command::rawUsage() const
{
DOTRACE("Tcl::Command::rawUsage");
  fstring result(rep->cmdName);
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

Tcl::Command* Tcl::Command::lookup(const char* name)
{
DOTRACE("Tcl::Command::lookup");
  return commandTable()[name];
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
