///////////////////////////////////////////////////////////////////////
//
// gtexttcl.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Jul  1 12:30:38 1999
// written: Fri Jan 18 16:07:01 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GTEXTTCL_CC_DEFINED
#define GTEXTTCL_CC_DEFINED

#include "visx/gtext.h"

#include "tcl/fieldpkg.h"

#include "util/objfactory.h"

#include "util/trace.h"

extern "C"
int Gtext_Init(Tcl_Interp* interp)
{
DOTRACE("Gtext_Init");

  Tcl::Pkg* pkg = new Tcl::Pkg(interp, "Gtext", "$Revision$");
  Tcl::defGenericObjCmds<Gtext>(pkg);
  Tcl::defAllFields(pkg, Gtext::classFields());

  Util::ObjFactory::theOne().registerCreatorFunc(&Gtext::make);

  return pkg->initStatus();
}

static const char vcid_gtexttcl_cc[] = "$Header$";
#endif // !GTEXTTCL_CC_DEFINED
