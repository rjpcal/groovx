///////////////////////////////////////////////////////////////////////
//
// fishtcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Sep 29 12:00:53 1999
// written: Mon Sep 10 17:17:40 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FISHTCL_CC_DEFINED
#define FISHTCL_CC_DEFINED

#include "visx/fish.h"

#include "tcl/fieldpkg.h"
#include "tcl/tracertcl.h"

#include "util/objfactory.h"

#include "util/trace.h"

namespace
{
  Util::UID makeFish(const char* spline_file, const char* coord_file,
                     int index)
  {
    Ref<Fish> obj(Fish::makeFromFiles(spline_file, coord_file, index));
    return obj.id();
  }
}

extern "C"
int Fish_Init(Tcl_Interp* interp)
{
DOTRACE("Fish_Init");

  Tcl::Pkg* pkg = new Tcl::Pkg(interp, "Fish", "$Revision$");

  Tcl::defTracing(pkg, Fish::tracer);

  Tcl::defGenericObjCmds<Fish>(pkg);

  pkg->defVec( "make", "spline_file coord_file index", &makeFish );

  Tcl::defAllFields(pkg, Fish::classFields());

  Util::ObjFactory::theOne().registerCreatorFunc(&Fish::make);

  return pkg->initStatus();
}

static const char vcid_fishtcl_cc[] = "$Header$";
#endif // !FISHTCL_CC_DEFINED
