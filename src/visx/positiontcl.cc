///////////////////////////////////////////////////////////////////////
//
// positiontcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Mar 13 12:53:34 1999
// written: Wed Jul 18 11:13:31 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef POSITIONTCL_CC_DEFINED
#define POSITIONTCL_CC_DEFINED

#include "position.h"

#include "tcl/fieldpkg.h"

#define NO_TRACE
#include "util/trace.h"

extern "C"
int Pos_Init(Tcl_Interp* interp)
{
DOTRACE("Pos_Init");

  Tcl::Pkg* pkg = new Tcl::Pkg(interp, "Pos", "$Revision$");
  Tcl::defFieldContainer<Position>(pkg);

  return pkg->initStatus();
}

static const char vcid_positiontcl_cc[] = "$Header$";
#endif // !POSITIONTCL_CC_DEFINED
