///////////////////////////////////////////////////////////////////////
//
// tclcmd.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Jun 11 14:50:58 1999
// written: Mon Jan 21 14:30:59 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLCMD_CC_DEFINED
#define TCLCMD_CC_DEFINED

#include "tcl/tclcmd.h"

#include "system/demangle.h"

#include "tcl/tclerror.h"
#include "tcl/tclsafeinterp.h"

#include "util/strings.h"

#include <tcl.h>
#include <exception>
#include <typeinfo>

#define TRACE_USE_COUNT

#ifdef TRACE_USE_COUNT
#  include <fstream.h>
#endif

#include "util/trace.h"
#include "util/debug.h"

class HelpCmd;

///////////////////////////////////////////////////////////////////////
//
// File scope definitions
//
///////////////////////////////////////////////////////////////////////

namespace
{
  inline void errMessage(Tcl_Interp* interp, const fstring& cmd_name,
                         const char* err_msg)
    {
      Tcl_AppendStringsToObj(Tcl_GetObjResult(interp),
                             cmd_name.c_str(), ": ", err_msg, (char *) NULL);
    }

  inline void errMessage(Tcl_Interp* interp, const fstring& cmd_name,
                         const std::type_info& exc_type, const char* what=0)
    {
      fstring msg = "an error of type ";
      msg.append( demangle_cstr(exc_type.name()) );
      msg.append( " occurred" );
      if (what)
        {
          msg.append( ": " );
          msg.append( what );
        }
      errMessage(interp, cmd_name, msg.c_str());
    }

  inline Tcl::Command* lookupCmd(Tcl_Interp* interp, const char* cmd_name)
    {
      Tcl_CmdInfo cmd_info;
      int result = Tcl_GetCommandInfo(interp, const_cast<char*>(cmd_name),
                                      &cmd_info);

      if (result == 0)
        return 0;

      return dynamic_cast<Tcl::Command*>(
              static_cast<Tcl::Command*>(cmd_info.objClientData));
    }

#ifdef TRACE_USE_COUNT
  STD_IO::ofstream* USE_COUNT_STREAM = new STD_IO::ofstream("tclprof.out");
#endif

  HelpCmd* theHelpCmd = 0;
  bool firstTime = true;
  // we need this extra 'firstTime' flag to avoid endless recursion
  // when trying to create theHelpCmd (i.e., otherwise the constructor
  // for HelpCmd will also try to create theHelpCmd, etc...)
}

///////////////////////////////////////////////////////////////////////
//
// Tcl::Dispatcher
//
///////////////////////////////////////////////////////////////////////

Tcl::Dispatcher::~Dispatcher() {}

///////////////////////////////////////////////////////////////////////
//
// Tcl::DefaultDispatcher
//
///////////////////////////////////////////////////////////////////////

namespace Tcl
{
  class DefaultDispatcher : public Dispatcher
  {
  public:
    virtual void dispatch(Tcl_Interp* interp,
                          unsigned int objc, Tcl_Obj* const objv[],
                          Tcl::Command& cmd)
    {
      Tcl::Context ctx(interp, objc, objv);
      cmd.invoke(ctx);
    }
  };
}

namespace
{
  shared_ptr<Tcl::DefaultDispatcher>
    theDefaultDispatcher(new Tcl::DefaultDispatcher);
}

///////////////////////////////////////////////////////////////////////
//
// HelpCmd class definition
//
///////////////////////////////////////////////////////////////////////

class HelpCmd : public Tcl::Command
{
public:
  HelpCmd(Tcl_Interp* interp) :
    Tcl::Command(interp, "?", "commandName", 2, 2) {}

protected:
  virtual void invoke(Tcl::Context& ctx)
  {
    const char* cmd_name = ctx.getValFromArg(1, TypeCue<const char*>());

    Tcl::Command* cmd = lookupCmd(ctx.interp(), cmd_name);

    if (cmd == 0)
      throw Util::Error("no such Tcl::Command");

    ctx.setResult(cmd->usage());
  }
};

///////////////////////////////////////////////////////////////////////
//
// Tcl::Command::Impl class definition
//
///////////////////////////////////////////////////////////////////////

class Tcl::Command::Impl
{
public:
  Impl(Tcl::Command* owner, Tcl_Interp* interp,
       const char* cmd_name, const char* usage,
       int objc_min, int objc_max, bool exact_objc) :
    itsOwner(owner),
    itsInterp(interp),
    itsDispatcher(theDefaultDispatcher),
    itsUsage(usage ? usage : ""),
    itsObjcMin(objc_min < 0 ? 0 : (unsigned int) objc_min),
    itsObjcMax( (objc_max > 0) ? (unsigned int) objc_max : itsObjcMin),
    itsExactObjc(exact_objc),
    itsCmdName(cmd_name),
    itsUseCount(0),
    itsOverload(0),
    itsPrevOverload(0)
  {
    registerCmdProc(interp);

    if (firstTime)
      {
        firstTime = false;
        theHelpCmd = new HelpCmd(interp);
      }
  }

  ~Impl()
    {
#ifdef TRACE_USE_COUNT
      if (USE_COUNT_STREAM->good())
        {
          *USE_COUNT_STREAM << itsCmdName << " "
                            << itsUseCount << STD_IO::endl;
        }
#endif

      removeSelfFromOverloads();

      // We must check if the Tcl_Interp* has been tagged for deletion
      // already, since if it is then we must not attempt to use it to
      // delete a Tcl command (this results in "called Tcl_HashEntry
      // on deleted table"). Not deleting the command in that case
      // does not cause a resource leak, however, since the Tcl_Interp
      // as part if its own destruction will delete all commands
      // associated with it.

      if ( !Tcl_InterpDeleted(itsInterp) )
        {
          try
            {
              Tcl_DeleteCommand(itsInterp,
                                const_cast<char*>(itsCmdName.c_str()));
            }
          catch (Util::Error& err)
            { DebugEvalNL(err.msg_cstr()); }
          catch (...)
            { DebugPrintNL("an unknown error occurred"); }
        }
    }

  fstring usage() const
  {
    return findFirstOverload()->buildUsage();
  }

  fstring buildUsage() const
  {
    fstring result;

    result.append("\t").append(itsCmdName);

    if (!itsUsage.is_empty())
      {
        result.append(" ").append(itsUsage);
      }

    if ( itsOverload != 0 )
      result.append("\n").append(itsOverload->buildUsage());

    return result;
  }

  const fstring& cmdName() const { return itsCmdName; }

private:
  void registerCmdProc(Tcl_Interp* interp)
  {
    Tcl::Command* previousCmd = lookupCmd(interp, itsCmdName.c_str());

    if ( previousCmd == 0 )
      {
        Tcl_CreateObjCommand(interp,
                             const_cast<char*>(itsCmdName.c_str()),
                             invokeCallback,
                             static_cast<ClientData>(itsOwner),
                             (Tcl_CmdDeleteProc*) NULL);
      }
    else
      {
        addAsOverloadOf(previousCmd);
      }
  }

  bool isOverloaded() const
    {
      return (itsOverload != 0) || (itsPrevOverload != 0);
    }

  const Impl* findFirstOverload() const
    {
      const Impl* first = this;
      while (first->itsPrevOverload != 0)
        first = first->itsPrevOverload;

      return first;
    }

  void addAsOverloadOf(Command* other)
  {
    Impl* last = other->itsImpl;

    while (last->itsOverload != 0)
      last = last->itsOverload;

    last->itsOverload = this;
    this->itsPrevOverload = last;
  }

  void removeSelfFromOverloads()
  {
    if (itsOverload)
      {
        itsOverload->itsPrevOverload = this->itsPrevOverload;
      }

    if (itsPrevOverload)
      {
        itsPrevOverload->itsOverload = this->itsOverload;
      }
  }

  bool allowsObjc(unsigned int objc) const
    {
      if (itsExactObjc)
        {
          return (objc == itsObjcMin || objc == itsObjcMax);
        }

      return (objc >= itsObjcMin && objc <= itsObjcMax);
    }

  bool rejectsObjc(unsigned int objc) const { return !allowsObjc(objc); }

  void onInvoke() { ++itsUseCount; }

  Impl* getOverload() const { return itsOverload; }

  fstring warnUsage()
  {
    fstring warning("wrong # args: should be ");

    if ( getOverload()==0 )
      {
        appendFullUsage(warning);
      }
    else
      {
        warning.append("one of:");
        Impl* cmd = this;
        while ( cmd != 0 )
          {
            warning.append("\n\t");
            cmd->appendFullUsage(warning);
            cmd = cmd->getOverload();
          }
      }

    return warning;
  }

  void appendFullUsage(fstring& str)
    {
      if (!itsUsage.is_empty())
        {
          str.append("\"", itsCmdName, " ");
          str.append(itsUsage, "\"");
        }
      else
        {
          str.append("\"", itsCmdName, "\"");
        }
    }

  /// The procedure that is actually registered with the Tcl C API.
  static int invokeCallback(ClientData clientData, Tcl_Interp* interp,
                            int objc, Tcl_Obj *const objv[]);

private:
  Impl(const Impl&);
  Impl& operator=(const Impl&);

  // These are set once per command object
  Tcl::Command* const itsOwner;
  Tcl_Interp* const itsInterp;
public:
  shared_ptr<Tcl::Dispatcher> itsDispatcher;
private:
  const fstring itsUsage;
  const unsigned int itsObjcMin;
  const unsigned int itsObjcMax;
  const bool itsExactObjc;
  const fstring itsCmdName;
  int itsUseCount;
  Impl* itsOverload;
  Impl* itsPrevOverload;
};


///////////////////////////////////////////////////////////////////////
//
// Tcl::Command::Impl member definitions
//
///////////////////////////////////////////////////////////////////////

int Tcl::Command::Impl::invokeCallback(ClientData clientData, Tcl_Interp* interp,
                                       int s_objc, Tcl_Obj *const objv[])
{
DOTRACE("Tcl::Command::Impl::invokeCallback");

  Impl* theImpl = static_cast<Tcl::Command*>(clientData)->itsImpl;

  theImpl->onInvoke();

  Assert(s_objc >= 0);

  unsigned int objc = (unsigned int) s_objc;

  // Look for an overload that matches...
  while ( theImpl->rejectsObjc(objc) )
    {
      theImpl = theImpl->getOverload();

      // If we run out of potential overloads, abort the command.
      if ( theImpl == 0 )
        {
          Impl* originalImpl = static_cast<Tcl::Command*>(clientData)->itsImpl;
          Tcl::Interp safeIntp(interp);
          safeIntp.resetResult();
          safeIntp.appendResult(originalImpl->warnUsage().c_str());
          return TCL_ERROR;
        }
    }

  // ...and try the matching overload and catch all possible exceptions
  try
    {
      theImpl->itsDispatcher->dispatch(interp, objc, objv, *(theImpl->itsOwner));
      return TCL_OK;
    }
  catch (Util::Error& err)
    {
      DebugPrintNL("catch (Util::Error&)");
      if ( !err.msg().is_empty() )
        {
          errMessage(interp, theImpl->cmdName(), err.msg_cstr());
        }
      else
        {
          errMessage(interp, theImpl->cmdName(), typeid(err));
        }
    }
  catch (std::exception& err)
    {
      errMessage(interp, theImpl->cmdName(), typeid(err), err.what());
    }
  catch (...)
    {
      DebugPrintNL("catch (...)");
      errMessage(interp, theImpl->cmdName(),
                 "an error of unknown type occurred");
    }

  return TCL_ERROR;
}

///////////////////////////////////////////////////////////////////////
//
// Tcl::Command member definitions
//
///////////////////////////////////////////////////////////////////////

Tcl::Command::~Command()
{
DOTRACE("Tcl::Command::~Command");
  delete itsImpl;
}

Tcl::Command::Command(Tcl_Interp* interp,
                      const char* cmd_name, const char* usage,
                      int objc_min, int objc_max, bool exact_objc) :
  itsImpl(new Impl(this, interp, cmd_name, usage,
                   objc_min, objc_max, exact_objc))
{
DOTRACE("Tcl::Command::Command");
}

const fstring& Tcl::Command::name() const
{
DOTRACE("Tcl::Command::name");
  return itsImpl->cmdName();
}

fstring Tcl::Command::usage() const
{
DOTRACE("Tcl::Command::usage");
  return itsImpl->usage();
}

shared_ptr<Tcl::Dispatcher> Tcl::Command::getDispatcher() const
{
DOTRACE("Tcl::Command::getDispatcher");
  return itsImpl->itsDispatcher;
}

void Tcl::Command::setDispatcher(shared_ptr<Tcl::Dispatcher> dpx)
{
DOTRACE("Tcl::Command::setDispatcher");
  itsImpl->itsDispatcher = dpx;
}


///////////////////////////////////////////////////////////////////////
//
// Tcl::Context member definitions
//
///////////////////////////////////////////////////////////////////////

Tcl::Context::Context(Tcl_Interp* interp,
                      unsigned int objc, Tcl_Obj* const* objv) :
  itsInterp(interp),
  itsObjc(objc),
  itsObjv(objv)
{}

Tcl::Context::~Context()
{}

void Tcl::Context::setObjResult(const Tcl::ObjPtr& obj)
{
DOTRACE("Tcl::Context::setObjResult");
  Tcl_SetObjResult(itsInterp, obj.obj());
}

static const char vcid_tclcmd_cc[] = "$Header$";
#endif // !TCLCMD_CC_DEFINED
