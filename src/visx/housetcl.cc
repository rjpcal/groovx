///////////////////////////////////////////////////////////////////////
//
// housetcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Sep 13 15:14:19 1999
// written: Wed Jul 18 09:52:13 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef HOUSETCL_CC_DEFINED
#define HOUSETCL_CC_DEFINED

#include "house.h"

#include "tcl/fieldpkg.h"

#define NO_TRACE
#include "util/trace.h"


extern "C"
int House_Init(Tcl_Interp* interp) {
DOTRACE("House_Init");

  Tcl::TclItemPkg* pkg = new Tcl::TclItemPkg(interp, "House", "$Revision$");
  Tcl::defFieldContainer<House>(pkg);

  return pkg->initStatus();
}

static const char vcid_housetcl_cc[] = "$Header$";
#endif // !HOUSETCL_CC_DEFINED
