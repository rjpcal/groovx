///////////////////////////////////////////////////////////////////////
//
// tclcmd.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Jun 11 14:50:58 1999
// written: Wed Aug  8 19:00:53 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLCMD_CC_DEFINED
#define TCLCMD_CC_DEFINED

#include "tcl/tclcmd.h"

#include "system/demangle.h"

#include "tcl/tclerror.h"
#include "tcl/tclvalue.h"

#include "util/strings.h"

#include <tcl.h>
#include <exception>
#include <typeinfo>

#define TRACE_USE_COUNT

#ifdef TRACE_USE_COUNT
#  include <fstream.h>
#endif

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

class HelpCmd;

///////////////////////////////////////////////////////////////////////
//
// File scope definitions
//
///////////////////////////////////////////////////////////////////////

namespace
{
  inline void errMessage(Tcl_Interp* interp, const fixed_string& cmd_name,
                         const char* err_msg)
    {
      Tcl_AppendStringsToObj(Tcl_GetObjResult(interp),
                             cmd_name.c_str(), ": ", err_msg, (char *) NULL);
    }

  inline void errMessage(Tcl_Interp* interp, const fixed_string& cmd_name,
                         const std::type_info& exc_type, const char* what=0)
    {
      fixed_string msg = "an error of type ";
      msg += demangle_cstr(exc_type.name());
      msg += " occurred";
      if (what)
        {
          msg += ": ";
          msg += what;
        }
      errMessage(interp, cmd_name, msg.c_str());
    }

  inline Tcl::TclCmd* lookupCmd(Tcl_Interp* interp, const char* cmd_name)
    {
      Tcl_CmdInfo cmd_info;
      int result = Tcl_GetCommandInfo(interp, const_cast<char*>(cmd_name),
                                      &cmd_info);

      if (result == 0)
        return 0;

      return dynamic_cast<Tcl::TclCmd*>(
              static_cast<Tcl::TclCmd*>(cmd_info.objClientData));
    }

#ifdef TRACE_USE_COUNT
  STD_IO::ofstream* USE_COUNT_STREAM = new STD_IO::ofstream("tclprof.out");
#endif

  HelpCmd* helpCmd = 0;
  bool firstTime = true;
}

///////////////////////////////////////////////////////////////////////
//
// HelpCmd class definition
//
///////////////////////////////////////////////////////////////////////

class HelpCmd : public Tcl::TclCmd {
public:
  HelpCmd(Tcl_Interp* interp) :
    Tcl::TclCmd(interp, "?", "commandName", 2, 2) {}

protected:
  virtual void invoke(Tcl::Context& ctx)
  {
    const char* cmd_name = ctx.getCstringFromArg(1);

    Tcl::TclCmd* cmd = lookupCmd(ctx.interp(), cmd_name);

    if (cmd == 0)
      throw Util::Error("no such TclCmd");

    ctx.setResult(cmd->usage());
  }
};

///////////////////////////////////////////////////////////////////////
//
// TclCmd::Impl class definition
//
///////////////////////////////////////////////////////////////////////

class Tcl::TclCmd::Impl {
public:
  Impl(Tcl::TclCmd* owner, Tcl_Interp* interp,
       const char* cmd_name, const char* usage,
       int objc_min, int objc_max, bool exact_objc) :
    itsOwner(owner),
    itsInterp(interp),
    itsUsage(usage),
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
        helpCmd = new HelpCmd(interp);
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

  const char* usage() const
  {
    static fixed_string result;

    result = findFirstOverload()->buildUsage();

    return result.c_str();
  }

  fixed_string buildUsage() const
  {
    fixed_string result;

    result.append("\t").append(itsCmdName);

    if (itsUsage && *itsUsage != '\0')
      {
        result.append(" ").append(itsUsage);
      }

    if ( itsOverload != 0 )
      result.append("\n").append(itsOverload->buildUsage());

    return result;
  }

  const char* name() const { return itsCmdName.c_str(); }

  const fixed_string& cmdName() const { return itsCmdName; }

private:
  void registerCmdProc(Tcl_Interp* interp)
  {
    Tcl::TclCmd* previousCmd = lookupCmd(interp, itsCmdName.c_str());

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

  void addAsOverloadOf(TclCmd* other)
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

  void warnUsage(Tcl_Interp* interp)
  {
    Tcl_Obj* result = Tcl_GetObjResult(interp);

    Tcl_AppendToObj(result, "wrong # args: should be ", -1);

    if ( getOverload()==0 )
      {
        appendFullUsage(result);
      }
    else
      {
        Tcl_AppendToObj(result, "one of:", -1);
        Impl* cmd = this;
        while ( cmd != 0 )
          {
            Tcl_AppendToObj(result, "\n\t", -1);
            cmd->appendFullUsage(result);
            cmd = cmd->getOverload();
          }
      }
  }

  void appendFullUsage(Tcl_Obj* result)
    {
      if (itsUsage && *itsUsage != '\0')
        {
          Tcl_AppendStringsToObj(result,
                                 "\"", const_cast<char*>(itsCmdName.c_str()),
                                 " ", itsUsage, "\"", (char*)0);
        }
      else
        {
          Tcl_AppendStringsToObj(result,
                                 "\"", const_cast<char*>(itsCmdName.c_str()),
                                 "\"", (char*)0);
        }
    }

  /// The procedure that is actually registered with the Tcl C API.
  static int invokeCallback(ClientData clientData, Tcl_Interp* interp,
                            int objc, Tcl_Obj *const objv[]);

private:

  Impl(const Impl&);
  Impl& operator=(const Impl&);

  // These are set once per command object
  Tcl::TclCmd* const itsOwner;
  Tcl_Interp* const itsInterp;
  const char* const itsUsage;
  const unsigned int itsObjcMin;
  const unsigned int itsObjcMax;
  const bool itsExactObjc;
  fixed_string itsCmdName;
  int itsUseCount;
  Impl* itsOverload;
  Impl* itsPrevOverload;
};


///////////////////////////////////////////////////////////////////////
//
// Tcl::TclCmd::Impl member definitions
//
///////////////////////////////////////////////////////////////////////

int Tcl::TclCmd::Impl::invokeCallback(ClientData clientData, Tcl_Interp* interp,
                                      int s_objc, Tcl_Obj *const objv[])
{
DOTRACE("Tcl::TclCmd::Impl::invokeCallback");

  Impl* theImpl = static_cast<Tcl::TclCmd*>(clientData)->itsImpl;

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
          Impl* originalImpl = static_cast<Tcl::TclCmd*>(clientData)->itsImpl;
          originalImpl->warnUsage(interp);
          return TCL_ERROR;
        }
    }

  // ...and try the matching overload and catch all possible exceptions
  try
    {
      theImpl->itsOwner->rawInvoke(interp, objc, objv);
      return TCL_OK;
    }
  catch (Util::Error& err)
    {
      DebugPrintNL("catch (Util::Error&)");
      if ( !string_literal(err.msg_cstr()).empty() )
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
  catch (const char* text)
    {
      fixed_string msg = "an error occurred: ";
      msg += text;
      errMessage(interp, theImpl->cmdName(), msg.c_str());
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
// TclCmd member definitions
//
///////////////////////////////////////////////////////////////////////

Tcl::TclCmd::~TclCmd() {
DOTRACE("Tcl::TclCmd::~TclCmd");
  delete itsImpl;
}

Tcl::TclCmd::TclCmd(Tcl_Interp* interp,
                    const char* cmd_name, const char* usage,
                    int objc_min, int objc_max, bool exact_objc) :
  itsImpl(new Impl(this, interp, cmd_name, usage,
                   objc_min, objc_max, exact_objc))
{
DOTRACE("Tcl::TclCmd::TclCmd");
}

const char* Tcl::TclCmd::name() const
{
DOTRACE("Tcl::TclCmd::name");
  return itsImpl->name();
}

const char* Tcl::TclCmd::usage() const
{
DOTRACE("Tcl::TclCmd::usage");
  return itsImpl->usage();
}

void Tcl::TclCmd::rawInvoke(Tcl_Interp* interp,
                            unsigned int objc, Tcl_Obj* const objv[])
{
DOTRACE("Tcl::TclCmd::rawInvoke");
  Context ctx(interp, objc, objv);
  invoke(ctx);
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

void Tcl::Context::setObjResult(Tcl_Obj* obj)
{
DOTRACE("Tcl::Context::setObjResult");
  Tcl_SetObjResult(itsInterp, obj);
}

static const char vcid_tclcmd_cc[] = "$Header$";
#endif // !TCLCMD_CC_DEFINED
