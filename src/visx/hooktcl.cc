///////////////////////////////////////////////////////////////////////
//
// hooktcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Oct  5 13:51:43 2000
// written: Wed Aug  8 20:16:40 2001
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

#include "tcl/tcllistobj.h"
#include "util/strings.h"

namespace HookTcl
{
  void hook(Tcl::Context& ctx)
  {
    Tcl::List result;

    fstring f("foo");
    result.append(f);

    f.append("bar");
    result.append(f);

    f.append(42);
    result.append(f);

    f.append(fstring("fstr"));
    result.append(f);

    result.append(f.length());

    ctx.setResult(result);
  }

  size_t memUsage() { return TOTAL; }
}

class HookPkg : public Tcl::Pkg {
public:
  HookPkg(Tcl_Interp* interp) :
    Tcl::Pkg(interp, "Hook", "$Revision$")
  {
    defVecRaw( "::hook", 0, "variable", HookTcl::hook );
    def( "::memUsage", 0, HookTcl::memUsage );
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
