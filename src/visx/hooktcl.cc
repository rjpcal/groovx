///////////////////////////////////////////////////////////////////////
//
// hooktcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Oct  5 13:51:43 2000
// written: Thu May 10 12:04:46 2001
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

class HookCmd : public Tcl::TclCmd {
public:
  HookCmd(Tcl_Interp* interp) :
	 Tcl::TclCmd(interp, "hook", "", 0, 10000, false) {}

protected:
  virtual void invoke() {
	 returnCstring("Hello, World!");
  }
};


class HookPkg : public Tcl::TclPkg {
public:
  HookPkg(Tcl_Interp* interp) :
	 Tcl::TclPkg(interp, "Hook", "$Revision$")
  {
	 addCommand(new HookCmd(interp));
  }
};

extern "C"
int Objtest_Init(Tcl_Interp* interp) {

  Tcl::TclPkg* pkg = new HookPkg(interp);

  return pkg->initStatus();
}

static const char vcid_hooktcl_cc[] = "$Header$";
#endif // !HOOKTCL_CC_DEFINED
