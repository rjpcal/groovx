///////////////////////////////////////////////////////////////////////
//
// tclcmd.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Jun 11 14:50:58 1999
// written: Thu Jul 12 13:06:59 2001
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
    fixed_string cmd_name(ctx.getCstringFromArg(1));
    Tcl_CmdInfo cmd_info;
    int result = Tcl_GetCommandInfo(ctx.interp(), cmd_name.data(), &cmd_info);
    if (result != 1)
      throw ErrorWithMsg("no such command");

    Tcl::TclCmd* cmd = dynamic_cast<Tcl::TclCmd*>(
                        static_cast<Tcl::TclCmd*>(cmd_info.objClientData));

    if (!cmd)
      throw ErrorWithMsg("no such TclCmd");

    dynamic_string cmd_usage(cmd_name.c_str());
    cmd_usage.append(" ").append(cmd->usage());
    ctx.setResult(cmd_usage.c_str());
  }
};


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
      dynamic_string msg = "an error of type ";
      msg += demangle_cstr(exc_type.name());
      msg += " occurred";
      if (what)
        {
          msg += ": ";
          msg += what;
        }
      errMessage(interp, cmd_name, msg.c_str());
    }

#ifdef TRACE_USE_COUNT
  STD_IO::ofstream* USE_COUNT_STREAM = new STD_IO::ofstream("tclprof.out");
#endif

  HelpCmd* helpCmd = 0;
  bool firstTime = true;
}

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
    itsUsage(usage),
    itsObjcMin(objc_min < 0 ? 0 : (unsigned int) objc_min),
    itsObjcMax( (objc_max > 0) ? (unsigned int) objc_max : itsObjcMin),
    itsExactObjc(exact_objc),
    itsCmdName(cmd_name),
    itsUseCount(0),
    itsOverload(0)
  {
    registerCmdProc(interp);
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
    }

  const char* usage() const { return itsUsage; }

  bool allowsObjc(unsigned int objc) const
    {
      if (itsExactObjc)
        {
          return (objc == itsObjcMin || objc == itsObjcMax);
        }

      return (objc >= itsObjcMin && objc <= itsObjcMax);
    }

  bool rejectsObjc(unsigned int objc) const { return !allowsObjc(objc); }

  const fixed_string& cmdName() const { return itsCmdName; }

  void onInvoke() { ++itsUseCount; }

  void addOverload(Tcl_Interp* interp, shared_ptr<TclCmd> other)
  {
	 itsOverload = other;

	 // We need to re-register our own CmdProc, since the overload
	 // probably blew ours away in its own constructor 
	 registerCmdProc(interp);
  }

  Impl* getOverload() const
  {
	 Tcl::TclCmd* overload = itsOverload.get();
	 return overload ? overload->itsImpl : 0;
  }

  void registerCmdProc(Tcl_Interp* interp)
  {
    Tcl_CreateObjCommand(interp,
                         const_cast<char*>(itsCmdName.c_str()),
                         invokeCallback,
                         static_cast<ClientData>(itsOwner),
                         (Tcl_CmdDeleteProc*) NULL);
  }

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

  /// The procedure that is actually registered with the Tcl C API.
  static int invokeCallback(ClientData clientData, Tcl_Interp* interp,
                            int objc, Tcl_Obj *const objv[]);

private:
  void appendFullUsage(Tcl_Obj* result)
	 {
		if (itsUsage)
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

  Impl(const Impl&);
  Impl& operator=(const Impl&);

  // These are set once per command object
  Tcl::TclCmd* const itsOwner;
  const char* const itsUsage;
  const unsigned int itsObjcMin;
  const unsigned int itsObjcMax;
  const bool itsExactObjc;
  fixed_string itsCmdName;
  int itsUseCount;
  shared_ptr<Tcl::TclCmd> itsOverload;
};

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

  if (firstTime)
    {
      firstTime = false;
      helpCmd = new HelpCmd(interp);
    }
}

const char* Tcl::TclCmd::usage()
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

void Tcl::TclCmd::addOverload(Tcl_Interp* interp, shared_ptr<TclCmd> other)
{
DOTRACE("Tcl::TclCmd::addOverload");
  itsImpl->addOverload(interp, other);
}

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
  catch (ErrorWithMsg& err)
    {
      DebugPrintNL("catch (ErrorWithMsg&)");
      if ( !string_literal(err.msg_cstr()).empty() )
        {
          errMessage(interp, theImpl->cmdName(), err.msg_cstr());
        }
      else
        {
          errMessage(interp, theImpl->cmdName(), typeid(err));
        }
    }
  catch (Error& err)
    {
      DebugPrintNL("catch (Error&)");
      errMessage(interp, theImpl->cmdName(), typeid(err));
    }
  catch (std::exception& err)
    {
      errMessage(interp, theImpl->cmdName(), typeid(err), err.what());
    }
  catch (const char* text)
    {
      dynamic_string msg = "an error occurred: ";
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
