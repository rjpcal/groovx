///////////////////////////////////////////////////////////////////////
//
// fishtcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Sep 29 12:00:53 1999
// written: Wed Jun  6 19:45:43 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FISHTCL_CC_DEFINED
#define FISHTCL_CC_DEFINED

#include "fish.h"

#include "tcl/fieldpkg.h"
#include "tcl/ioitempkg.h"
#include "tcl/tracertcl.h"

#include "util/objfactory.h"

namespace FishTcl {
  class FishCmd;
  class FishPkg;
}

class FishTcl::FishCmd : public Tcl::TclCmd {
public:
  FishCmd(Tcl_Interp* interp, const char* cmd_name) :
	 Tcl::TclCmd(interp, cmd_name, "spline_file coord_file index", 1, 4) {}
protected:
  virtual void invoke() {
	 if (objc() == 1) {
		IdItem<Fish> obj(Fish::make());
		returnInt(obj.id());
	 }
	 else if (objc() == 4) {
		const char* spline_file = arg(1).getCstring();
		const char* coord_file = arg(2).getCstring();
		int index = arg(3).getInt();

		IdItem<Fish> obj(Fish::makeFromFiles(spline_file, coord_file, index));
		returnInt(obj.id());
	 }
  }
};

class FishTcl::FishPkg : public Tcl::IoItemPkg<Fish> {
public:
  FishPkg(Tcl_Interp* interp) :
	 Tcl::IoItemPkg<Fish>(interp, "Fish", "$Revision$")
  {
	 Tcl::addTracing(this, Fish::tracer);

	 addCommand( new FishCmd(interp, "Fish::Fish") );
	 Tcl::declareAllFields(this, Fish::classFields());
  }
};

extern "C"
int Fish_Init(Tcl_Interp* interp) {

  Tcl::TclPkg* pkg = new FishTcl::FishPkg(interp);

  Util::ObjFactory::theOne().registerCreatorFunc(&Fish::make);

  return pkg->initStatus();
}

static const char vcid_fishtcl_cc[] = "$Header$";
#endif // !FISHTCL_CC_DEFINED
