///////////////////////////////////////////////////////////////////////
//
// gabortcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Oct  6 14:16:30 1999
// written: Wed Jul 18 09:51:54 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GABORTCL_CC_DEFINED
#define GABORTCL_CC_DEFINED

#include "gabor.h"

#include "tcl/fieldpkg.h"

#define NO_TRACE
#include "util/trace.h"

extern "C"
int Gabor_Init(Tcl_Interp* interp) {
DOTRACE("Gabor_Init");

  Tcl::TclItemPkg* pkg = new Tcl::TclItemPkg(interp, "Gabor", "$Revision$");
  Tcl::defFieldContainer<Gabor>(pkg);

  return pkg->initStatus();
}

static const char vcid_gabortcl_cc[] = "$Header$";
#endif // !GABORTCL_CC_DEFINED
