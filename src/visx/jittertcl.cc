///////////////////////////////////////////////////////////////////////
// jittertcl.cc
// Rob Peters
// created: Wed Apr  7 14:58:40 1999
// written: Wed Jul  7 14:07:42 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef JITTERTCL_CC_DEFINED
#define JITTERTCL_CC_DEFINED

#include "jittertcl.h"

#include <tcl.h>
#include <strstream.h>

#include "iomgr.h"
#include "jitter.h"
#include "poslist.h"
#include "listitempkg.h"

#define NO_TRACE
#include "trace.h"
#include "debug.h"

namespace JitterTcl {
  class SetJitterCmd;
  class JitterPkg;
}

//---------------------------------------------------------------------
//
// SetJitterCmd --
//
//---------------------------------------------------------------------

class JitterTcl::SetJitterCmd : public TclItemCmd<Jitter> {
public:
  SetJitterCmd(TclItemPkg* pkg, const char* cmd_name) :
	 TclItemCmd<Jitter>(pkg, cmd_name,
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

class JitterTcl::JitterPkg : public ListItemPkg<Jitter, PosList> {
public:
  JitterPkg(Tcl_Interp* interp) : 
	 ListItemPkg<Jitter, PosList>(interp, PosList::thePosList(), "Jitter", "2.3")
  {
	 addCommand( new SetJitterCmd(this, "Jitter::setJitter") );
  }
};

//---------------------------------------------------------------------
//
// Jitter_Init
//
//---------------------------------------------------------------------

int Jitter_Init(Tcl_Interp* interp) {
DOTRACE("Jitter_Init");

  new JitterTcl::JitterPkg(interp);

  FactoryRegistrar<IO, Jitter> registrar(IoFactory::theOne());

  return TCL_OK;
}

static const char vcid_jittertcl_cc[] = "$Header$";
#endif // !JITTERTCL_CC_DEFINED
