///////////////////////////////////////////////////////////////////////
//
// jittertcl.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Apr  7 14:58:40 1999
// written: Fri Nov 22 17:18:23 2002
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
