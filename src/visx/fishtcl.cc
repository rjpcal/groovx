///////////////////////////////////////////////////////////////////////
//
// fishtcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Sep 29 12:00:53 1999
// written: Thu May 10 12:04:48 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FISHTCL_CC_DEFINED
#define FISHTCL_CC_DEFINED

#include "fish.h"

#include "io/iofactory.h"

#include "tcl/fieldpkg.h"
#include "tcl/ioitempkg.h"
#include "tcl/tracertcl.h"

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
		Fish* p = Fish::make();
		IdItem<GrObj> obj(p, IdItem<GrObj>::Insert());
		returnInt(obj.id());
	 }
	 else if (objc() == 4) {
		const char* spline_file = arg(1).getCstring();
		const char* coord_file = arg(2).getCstring();
		int index = arg(3).getInt();
		
		Fish* p = Fish::makeFromFiles(spline_file, coord_file, index);
		
		IdItem<GrObj> obj(p, IdItem<GrObj>::Insert());
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

  IO::IoFactory::theOne().registerCreatorFunc(&Fish::make);

  return pkg->initStatus();
}

static const char vcid_fishtcl_cc[] = "$Header$";
#endif // !FISHTCL_CC_DEFINED
