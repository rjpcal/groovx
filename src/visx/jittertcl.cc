///////////////////////////////////////////////////////////////////////
//
// jittertcl.cc
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Apr  7 14:58:40 1999
// written: Fri Nov 10 17:27:02 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef JITTERTCL_CC_DEFINED
#define JITTERTCL_CC_DEFINED

#include "jitter.h"

#include "io/iofactory.h"

#include "tcl/listitempkg.h"

#define NO_TRACE
#include "util/trace.h"
#include "util/debug.h"

namespace JitterTcl {
  class SetJitterCmd;
  class JitterPkg;
}

//---------------------------------------------------------------------
//
// SetJitterCmd --
//
//---------------------------------------------------------------------

class JitterTcl::SetJitterCmd : public Tcl::TclItemCmd<Jitter> {
public:
  SetJitterCmd(Tcl::CTclItemPkg<Jitter>* pkg, const char* cmd_name) :
	 Tcl::TclItemCmd<Jitter>(pkg, cmd_name,
							  "posid x_jitter y_jitter r_jitter", 5, 5) {}
protected:
  virtual void invoke() {
	 Jitter* p = getItem();
	 double xj = getDoubleFromArg(2); p->setXJitter(xj);
	 double yj = getDoubleFromArg(3); p->setYJitter(yj);
	 double rj = getDoubleFromArg(4); p->setRJitter(rj);
	 returnVoid();
  }
};

///////////////////////////////////////////////////////////////////////
//
// JitterPkg class definition
//
///////////////////////////////////////////////////////////////////////

class JitterTcl::JitterPkg : public Tcl::ItemPkg<Jitter> {
public:
  JitterPkg(Tcl_Interp* interp) : 
	 Tcl::ItemPkg<Jitter>(interp, "Jitter", "$Revision$")
  {
	 addCommand( new SetJitterCmd(this, "Jitter::setJitter") );
  }
};

//---------------------------------------------------------------------
//
// Jitter_Init
//
//---------------------------------------------------------------------

extern "C"
int Jitter_Init(Tcl_Interp* interp) {
DOTRACE("Jitter_Init");

  Tcl::TclPkg* pkg = new JitterTcl::JitterPkg(interp);

  IO::IoFactory::theOne().registerCreatorFunc(&Jitter::make);

  return pkg->initStatus();
}

static const char vcid_jittertcl_cc[] = "$Header$";
#endif // !JITTERTCL_CC_DEFINED
