///////////////////////////////////////////////////////////////////////
//
// fishtcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Sep 29 12:00:53 1999
// written: Wed Jul 18 11:27:36 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FISHTCL_CC_DEFINED
#define FISHTCL_CC_DEFINED

#include "fish.h"

#include "tcl/fieldpkg.h"
#include "tcl/tclpkg.h"
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

class FishTcl::FishPkg : public Tcl::Pkg {
public:
  FishPkg(Tcl_Interp* interp) :
    Tcl::Pkg(interp, "Fish", "$Revision$")
  {
    Tcl::defTracing(this, Fish::tracer);

    Tcl::defGenericObjCmds<Fish>(this);

    defVec(&FishTcl::makeFish, "make", "spline_file coord_file index");

    Tcl::defAllFields(this, Fish::classFields());
  }
};

extern "C"
int Fish_Init(Tcl_Interp* interp)
{
  Tcl::Pkg* pkg = new FishTcl::FishPkg(interp);

  Util::ObjFactory::theOne().registerCreatorFunc(&Fish::make);

  return pkg->initStatus();
}

static const char vcid_fishtcl_cc[] = "$Header$";
#endif // !FISHTCL_CC_DEFINED
