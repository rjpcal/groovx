///////////////////////////////////////////////////////////////////////
//
// positiontcl.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Mar 13 12:53:34 1999
// written: Fri Jan 18 16:07:00 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef POSITIONTCL_CC_DEFINED
#define POSITIONTCL_CC_DEFINED

#include "visx/position.h"

#include "tcl/fieldpkg.h"

#include "util/trace.h"

extern "C"
int Position_Init(Tcl_Interp* interp)
{
DOTRACE("Position_Init");

  Tcl::Pkg* pkg = new Tcl::Pkg(interp, "Position", "$Revision$");
  Tcl::defFieldContainer<Position>(pkg);
  Tcl::defCreator<Position>(pkg);

  return pkg->initStatus();
}

static const char vcid_positiontcl_cc[] = "$Header$";
#endif // !POSITIONTCL_CC_DEFINED
