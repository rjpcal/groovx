///////////////////////////////////////////////////////////////////////
//
// jittertcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Apr  7 14:58:40 1999
// written: Mon Jul 16 10:32:50 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef JITTERTCL_CC_DEFINED
#define JITTERTCL_CC_DEFINED

#include "jitter.h"

#include "tcl/tclitempkg.h"
#include "tcl/objfunctor.h"

#include "util/objfactory.h"

#include "util/trace.h"

namespace JitterTcl
{
  class JitterPkg;
}

///////////////////////////////////////////////////////////////////////
//
// JitterPkg class definition
//
///////////////////////////////////////////////////////////////////////

class JitterTcl::JitterPkg : public Tcl::TclItemPkg {
public:
  JitterPkg(Tcl_Interp* interp) :
    Tcl::TclItemPkg(interp, "Jitter", "$Revision$")
  {
    Tcl::defGenericObjCmds<Jitter>(this);

    defVec( &Jitter::setJitter,
            "setJitter", "posid x_jitter y_jitter r_jitter" );
  }
};

//---------------------------------------------------------------------
//
// Jitter_Init
//
//---------------------------------------------------------------------

extern "C"
int Jitter_Init(Tcl_Interp* interp)
{
DOTRACE("Jitter_Init");

  Tcl::TclPkg* pkg = new JitterTcl::JitterPkg(interp);

  Util::ObjFactory::theOne().registerCreatorFunc(&Jitter::make);

  return pkg->initStatus();
}

static const char vcid_jittertcl_cc[] = "$Header$";
#endif // !JITTERTCL_CC_DEFINED
