///////////////////////////////////////////////////////////////////////
//
// tclcmd.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Jun 11 14:50:58 1999
// written: Wed Jul 11 21:05:52 2001
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
// File scope declarations
//
///////////////////////////////////////////////////////////////////////

class HelpCmd : public Tcl::TclCmd {
public:
  HelpCmd(Tcl_Interp* interp) :
    Tcl::TclCmd(interp, "?", "commandName", 2, 2) {}

protected:
  virtual void invoke(Context& ctx)
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

namespace
{
  inline void errMessage(Tcl_Interp* interp, Tcl_Obj* const objv[],
                         const char* err_msg)
    {
      Tcl_AppendObjToObj(Tcl_GetObjResult(interp), objv[0]);
      Tcl_AppendStringsToObj(Tcl_GetObjResult(interp),
                             ": ", err_msg, (char *) NULL);
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
  Impl(const char* cmd_name) : itsCmdName(cmd_name), itsUseCount(0) {}
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

  fixed_string itsCmdName;
  int itsUseCount;
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

Tcl::TclCmd::TclCmd(Tcl_Interp* interp, const char* cmd_name, const char* usage,
               int objc_min, int objc_max, bool exact_objc) :
  itsUsage(usage),
  itsObjcMin(objc_min),
  itsObjcMax( (objc_max > 0) ? objc_max : objc_min),
  itsExactObjc(exact_objc),
  itsImpl(new Impl(cmd_name))
{
DOTRACE("Tcl::TclCmd::TclCmd");
  Tcl_CreateObjCommand(interp,
                       const_cast<char *>(cmd_name),
                       invokeCallback,
                       static_cast<ClientData> (this),
                       (Tcl_CmdDeleteProc*) NULL);

  if (firstTime)
    {
      firstTime = false;
      helpCmd = new HelpCmd(interp);
    }
}

const char* Tcl::TclCmd::usage() {
DOTRACE("Tcl::TclCmd::usage");
  return itsUsage;
}


int Tcl::TclCmd::invokeCallback(ClientData clientData, Tcl_Interp* interp,
                                int objc, Tcl_Obj *const objv[]) {
DOTRACE("Tcl::TclCmd::invokeCallback");

  TclCmd* theCmd = static_cast<TclCmd*>(clientData);

  DebugEval((void *) theCmd);
  DebugEvalNL(typeid(*theCmd).name());

  ++(theCmd->itsImpl->itsUseCount);

  Context ctx(interp, objc, objv);

  DebugEval(objc);
  DebugEval(theCmd->itsObjcMin);
  DebugEvalNL(theCmd->itsObjcMax);

  // Check for bad argument count, if so abort the command and return
  // TCL_ERROR...
  if ( ((theCmd->itsExactObjc == true) &&
        (objc != theCmd->itsObjcMin && objc != theCmd->itsObjcMax))
       ||
       ((theCmd->itsExactObjc == false) &&
        (objc < theCmd->itsObjcMin || objc > theCmd->itsObjcMax)) )
    {
      Tcl_WrongNumArgs(ctx.itsInterp, 1, ctx.itsObjv,
                       const_cast<char*>(theCmd->itsUsage));
      ctx.itsResult = TCL_ERROR;
      return TCL_ERROR;
    }
  // ...otherwise if the argument count is OK, try the command and
  // catch all possible exceptions
  try {
    Context ctx(interp, objc, objv);
    theCmd->invoke(ctx);
  }
  catch (TclError& err) {
    DebugPrintNL("catch (TclError&)");
    if ( !string_literal(err.msg_cstr()).empty() ) {
      errMessage(interp, ctx.itsObjv, err.msg_cstr());
    }
    ctx.itsResult = TCL_ERROR;
  }
  catch (ErrorWithMsg& err) {
    DebugPrintNL("catch (ErrorWithMsg&)");
    if ( !string_literal(err.msg_cstr()).empty() ) {
      errMessage(interp, ctx.itsObjv, err.msg_cstr());
    }
    else {
      dynamic_string msg = "an error of type ";
      msg += demangle_cstr(typeid(err).name());
      msg += " occurred";
      errMessage(interp, ctx.itsObjv, msg.c_str());
    }
    ctx.itsResult = TCL_ERROR;
  }
  catch (Error& err) {
    DebugPrintNL("catch (Error&)");
    dynamic_string msg = "an error of type ";
    msg += demangle_cstr(typeid(err).name());
    msg += " occurred";
    errMessage(interp, ctx.itsObjv, msg.c_str());
    ctx.itsResult = TCL_ERROR;
  }
  catch (std::exception& err) {
    dynamic_string msg = "an error of type ";
    msg += demangle_cstr(typeid(err).name());
    msg += " occurred: ";
    msg += err.what();
    errMessage(interp, ctx.itsObjv, msg.c_str());
    ctx.itsResult = TCL_ERROR;
  }
  catch (const char* text) {
    dynamic_string msg = "an error occurred: ";
    msg += text;
    errMessage(interp, ctx.itsObjv, msg.c_str());
    ctx.itsResult = TCL_ERROR;
  }
  catch (...) {
    DebugPrintNL("catch (...)");
    errMessage(interp, ctx.itsObjv, "an error of unknown type occurred");
    ctx.itsResult = TCL_ERROR;
  }

  DebugEvalNL(ctx.itsResult == TCL_OK);

  return ctx.itsResult;
}


///////////////////////////////////////////////////////////////////////
//
// Tcl::TclCmd::Context member definitions
//
///////////////////////////////////////////////////////////////////////

Tcl::TclCmd::Context::Context(Tcl_Interp* interp,
                              int objc,
                              Tcl_Obj* const* objv) :
  itsInterp(interp),
  itsObjc(objc),
  itsObjv(objv),
  itsResult(TCL_OK)
{}

void Tcl::TclCmd::Context::setObjResult(Tcl_Obj* obj) {
DOTRACE("Tcl::TclCmd::setObjResult");
  Tcl_SetObjResult(itsInterp, obj);
  itsResult = TCL_OK;
}

static const char vcid_tclcmd_cc[] = "$Header$";
#endif // !TCLCMD_CC_DEFINED
