///////////////////////////////////////////////////////////////////////
//
// hooktcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Oct  5 13:51:43 2000
// written: Sat Aug 25 21:25:37 2001
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
#include <cstdio>

void* operator new(size_t bytes)
{
  TOTAL += bytes;
  printf("%d bytes  ", (int) bytes);
  Util::Trace::printStackTrace();
  return malloc(bytes);
}

void operator delete(void* space)
{
  free(space);
}
#endif

#include "tcl/tcltimer.h"
#include <iostream>

namespace HookTcl
{
  Tcl::Timer aTimer(1000);

  class H : public Util::Object
  {
  public:
    H() { aTimer.setRepeating(true); aTimer.sigTimeOut.connect(this, &H::doit); }

    static H* make() { return new H; }

    void doit() { std::cout << "H::doit!" << std::endl; }
  };

  Util::Ref<H> anH(H::make());

  void hook()
  {
    aTimer.schedule();
  }

  size_t memUsage() { return TOTAL; }
}

extern "C"
int Hook_Init(Tcl_Interp* interp)
{
  Tcl::Pkg* pkg = new Tcl::Pkg(interp, "Hook", "$Revision$");
  pkg->def( "::hook", "variable", HookTcl::hook );
  pkg->def( "::memUsage", 0, HookTcl::memUsage );

  return pkg->initStatus();
}

static const char vcid_hooktcl_cc[] = "$Header$";
#endif // !HOOKTCL_CC_DEFINED
