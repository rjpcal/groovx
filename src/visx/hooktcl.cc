///////////////////////////////////////////////////////////////////////
//
// hooktcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Oct  5 13:51:43 2000
// written: Thu Jul 12 12:27:02 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef HOOKTCL_CC_DEFINED
#define HOOKTCL_CC_DEFINED

#include "tcl/tclcmd.h"
#include "tcl/tclpkg.h"

#include <tcl.h>

#define LOCAL_TRACE
#include "util/trace.h"
#define LOCAL_DEBUG
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////

namespace {
  size_t TOTAL = 0;
}

#if 0
#include <cstdlib>

void* operator new(size_t bytes)
{
  TOTAL += bytes;
  return malloc(bytes);
}

void operator delete(void* space)
{
  free(space);
}
#endif

class MemUsageCmd : public Tcl::TclCmd {
public:
  MemUsageCmd(Tcl_Interp* interp) :
    Tcl::TclCmd(interp, "memUsage", "", 1, 1, false) {}

protected:
  virtual void invoke(Context& ctx) {
    ctx.setResult(TOTAL);
  }
};

#include "tcl/tclveccmd.h"

class HookCmd : public Tcl::VecCmd {
public:
  HookCmd(Tcl_Interp* interp) :
    Tcl::VecCmd(interp, "hook", "", 1, 0, 10000, false) {}

protected:
  virtual void invoke(Context& ctx)
  {
    int isum = 0;

    for (int i = 1; i < ctx.objc(); ++i)
      {
        isum += ctx.getIntFromArg(i);
      }

    ctx.setResult(isum);
  }
};


class HookPkg : public Tcl::TclPkg {
public:
  HookPkg(Tcl_Interp* interp) :
    Tcl::TclPkg(interp, "Hook", "$Revision$")
  {
    addCommand(new HookCmd(interp));
    addCommand(new MemUsageCmd(interp));
  }
};

extern "C"
int Hook_Init(Tcl_Interp* interp) {

  Tcl::TclPkg* pkg = new HookPkg(interp);

  return pkg->initStatus();
}

static const char vcid_hooktcl_cc[] = "$Header$";
#endif // !HOOKTCL_CC_DEFINED
