///////////////////////////////////////////////////////////////////////
//
// fishtcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Sep 29 12:00:53 1999
// written: Fri Jul 13 15:23:00 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FISHTCL_CC_DEFINED
#define FISHTCL_CC_DEFINED

#include "fish.h"

#include "tcl/fieldpkg.h"
#include "tcl/genericobjpkg.h"
#include "tcl/objfunctor.h"
#include "tcl/tracertcl.h"

#include "util/objfactory.h"

namespace FishTcl
{
  Util::UID makeFish(const char* spline_file, const char* coord_file,
                     int index)
  {
    Ref<Fish> obj(Fish::makeFromFiles(spline_file, coord_file, index));
    return obj.id();
  }

  class FishPkg;
}

class FishTcl::FishPkg : public Tcl::GenericObjPkg<Fish> {
public:
  FishPkg(Tcl_Interp* interp) :
    Tcl::GenericObjPkg<Fish>(interp, "Fish", "$Revision$")
  {
    Tcl::addTracing(this, Fish::tracer);

    Tcl::defVec(this, &FishTcl::makeFish,
                "Fish::make", "spline_file coord_file index");

    Tcl::declareAllFields(this, Fish::classFields());
  }
};

extern "C"
int Fish_Init(Tcl_Interp* interp)
{
  Tcl::TclPkg* pkg = new FishTcl::FishPkg(interp);

  Util::ObjFactory::theOne().registerCreatorFunc(&Fish::make);

  return pkg->initStatus();
}

static const char vcid_fishtcl_cc[] = "$Header$";
#endif // !FISHTCL_CC_DEFINED
