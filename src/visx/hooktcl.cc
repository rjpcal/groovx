///////////////////////////////////////////////////////////////////////
//
// hooktcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Oct  5 13:51:43 2000
// written: Mon Jul 16 12:15:01 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef HOOKTCL_CC_DEFINED
#define HOOKTCL_CC_DEFINED

#include "tcl/tclitempkg.h"

#define LOCAL_TRACE
#include "util/trace.h"
#define LOCAL_DEBUG
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////

namespace
{
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

namespace HookTcl
{
  void hook(Tcl::Context& ctx)
  {
    int isum = 0;

    for (int i = 1; i < ctx.objc(); ++i)
      {
        isum += ctx.getIntFromArg(i);
      }

    ctx.setResult(isum);
  }

  size_t memUsage() { return TOTAL; }
}

class HookPkg : public Tcl::TclItemPkg {
public:
  HookPkg(Tcl_Interp* interp) :
    Tcl::TclItemPkg(interp, "Hook", "$Revision$")
  {
    defVecRaw( HookTcl::hook, "hook", "variable", 2 );
    def( HookTcl::memUsage, "memUsage", 0 );
  }
};

extern "C"
int Hook_Init(Tcl_Interp* interp) {

  Tcl::TclPkg* pkg = new HookPkg(interp);

  return pkg->initStatus();
}

static const char vcid_hooktcl_cc[] = "$Header$";
#endif // !HOOKTCL_CC_DEFINED
