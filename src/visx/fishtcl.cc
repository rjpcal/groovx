///////////////////////////////////////////////////////////////////////
//
// fishtcl.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Sep 29 12:00:53 1999
// written: Thu Oct 26 09:15:14 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FISHTCL_CC_DEFINED
#define FISHTCL_CC_DEFINED

#include "objlist.h"
#include "fish.h"

#include "io/iofactory.h"

#include "tcl/listitempkg.h"
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
		ItemWithId<GrObj> obj(p, ItemWithId<GrObj>::Insert());
		returnInt(obj.id());
	 }
	 else if (objc() == 4) {
		const char* spline_file = arg(1).getCstring();
		const char* coord_file = arg(2).getCstring();
		int index = arg(3).getInt();
		
		Fish* p = Fish::makeFromFiles(spline_file, coord_file, index);
		
		ItemWithId<GrObj> obj(p, ItemWithId<GrObj>::Insert());
		returnInt(obj.id());
	 }
  }
};

class FishTcl::FishPkg : public Tcl::AbstractListItemPkg<Fish, ObjList> {
public:
  FishPkg(Tcl_Interp* interp) :
	 Tcl::AbstractListItemPkg<Fish, ObjList>(interp, ObjList::theObjList(),
														  "Fish", "$Revision$")
  {
	 Tcl::addTracing(this, Fish::tracer);

	 addCommand( new FishCmd(interp, "Fish::Fish") );
	 declareAllProperties();
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
