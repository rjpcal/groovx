///////////////////////////////////////////////////////////////////////
//
// hooktcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Oct  5 13:51:43 2000
// written: Wed Jul 18 11:33:30 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef HOOKTCL_CC_DEFINED
#define HOOKTCL_CC_DEFINED

#include "tcl/tclpkg.h"

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

#include <tcl.h>

namespace HookTcl
{
  void hook(Tcl::Context& ctx)
  {
    const char* proc = ctx.getValFromArg(1, TypeCue<const char*>());
    Tcl_CmdInfo info;
    int result = Tcl_GetCommandInfo(ctx.interp(), (char*)proc, &info);
    ctx.setResult(result);
  }

  size_t memUsage() { return TOTAL; }
}

class HookPkg : public Tcl::Pkg {
public:
  HookPkg(Tcl_Interp* interp) :
    Tcl::Pkg(interp, "Hook", "$Revision$")
  {
    defVecRaw( HookTcl::hook, "::hook", "variable", 1 );
    def( HookTcl::memUsage, "::memUsage", 0 );
  }
};

extern "C"
int Hook_Init(Tcl_Interp* interp)
{
  Tcl::Pkg* pkg = new HookPkg(interp);

  return pkg->initStatus();
}

static const char vcid_hooktcl_cc[] = "$Header$";
#endif // !HOOKTCL_CC_DEFINED
