///////////////////////////////////////////////////////////////////////
//
// jittertcl.cc
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Wed Apr  7 14:58:40 1999
// written: Wed Mar 19 12:46:30 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef JITTERTCL_CC_DEFINED
#define JITTERTCL_CC_DEFINED

#include "visx/jitter.h"

#include "tcl/objpkg.h"
#include "tcl/tclpkg.h"

#include "util/objfactory.h"

#include "util/trace.h"

extern "C"
int Jitter_Init(Tcl_Interp* interp)
{
DOTRACE("Jitter_Init");

  Tcl::Pkg* pkg = new Tcl::Pkg(interp, "Jitter", "$Revision$");
  Tcl::defGenericObjCmds<Jitter>(pkg);

  pkg->defVec( "setJitter", "posid x_jitter y_jitter r_jitter",
               &Jitter::setJitter );

  Util::ObjFactory::theOne().registerCreatorFunc(&Jitter::make);

  return pkg->initStatus();
}

static const char vcid_jittertcl_cc[] = "$Header$";
#endif // !JITTERTCL_CC_DEFINED
