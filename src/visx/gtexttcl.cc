///////////////////////////////////////////////////////////////////////
//
// gtexttcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Jul  1 12:30:38 1999
// written: Sat Aug 25 21:50:01 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GTEXTTCL_CC_DEFINED
#define GTEXTTCL_CC_DEFINED

#include "gtext.h"

#include "tcl/tclpkg.h"

#include "util/objfactory.h"

#include "util/trace.h"

extern "C"
int Gtext_Init(Tcl_Interp* interp)
{
DOTRACE("Gtext_Init");

  Tcl::Pkg* pkg = new Tcl::Pkg(interp, "Gtext", "$Revision$");
  Tcl::defGenericObjCmds<Gtext>(pkg);

  pkg->defAttrib("text", &Gtext::getText, &Gtext::setText);
  pkg->defAttrib("strokeWidth", &Gtext::getStrokeWidth, &Gtext::setStrokeWidth);

  Util::ObjFactory::theOne().registerCreatorFunc(&Gtext::make);

  return pkg->initStatus();
}

static const char vcid_gtexttcl_cc[] = "$Header$";
#endif // !GTEXTTCL_CC_DEFINED
