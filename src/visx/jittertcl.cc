///////////////////////////////////////////////////////////////////////
//
// jittertcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Apr  7 14:58:40 1999
// written: Fri Jul 13 17:30:07 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef JITTERTCL_CC_DEFINED
#define JITTERTCL_CC_DEFINED

#include "jitter.h"

#include "tcl/genericobjpkg.h"
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

class JitterTcl::JitterPkg : public Tcl::GenericObjPkg<Jitter> {
public:
  JitterPkg(Tcl_Interp* interp) :
    Tcl::GenericObjPkg<Jitter>(interp, "Jitter", "$Revision$")
  {
    Tcl::defVec( this, &Jitter::setJitter,
                 "Jitter::setJitter", "posid x_jitter y_jitter r_jitter" );
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
