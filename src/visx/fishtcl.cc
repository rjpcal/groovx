///////////////////////////////////////////////////////////////////////
//
// fishtcl.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Sep 29 12:00:53 1999
// written: Tue Dec  7 19:02:53 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FISHTCL_CC_DEFINED
#define FISHTCL_CC_DEFINED

#include <tcl.h>

#include "iomgr.h"
#include "objlist.h"
#include "listitempkg.h"
#include "fish.h"

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
		Fish* p = new Fish();
		int objid = ObjList::theObjList().insert(p);
		returnInt(objid);
	 }
	 else if (objc() == 4) {
		const char* spline_file = arg(1).getCstring();
		const char* coord_file = arg(2).getCstring();
		int index = arg(3).getInt();
		
		Fish* p = new Fish(spline_file, coord_file, index);
		
		int objid = ObjList::theObjList().insert(p);
		returnInt(objid);
	 }
  }
};

class FishTcl::FishPkg : public Tcl::AbstractListItemPkg<Fish, ObjList> {
public:
  FishPkg(Tcl_Interp* interp) :
	 Tcl::AbstractListItemPkg<Fish, ObjList>(interp, ObjList::theObjList(),
														  "Fish", "1.1")
  {
	 addCommand( new FishCmd(interp, "Fish::Fish") );
	 declareAllProperties();
  }
};

extern "C" Tcl_PackageInitProc Fish_Init;

int Fish_Init(Tcl_Interp* interp) {

  new FishTcl::FishPkg(interp);

  FactoryRegistrar<IO, Fish>::registerWith(IoFactory::theOne());

  return TCL_OK;
}

static const char vcid_fishtcl_cc[] = "$Header$";
#endif // !FISHTCL_CC_DEFINED
