///////////////////////////////////////////////////////////////////////
//
// positiontcl.cc
// Rob Peters
// created: Sat Mar 13 12:53:34 1999
// written: Thu Mar 30 12:09:09 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef POSITIONTCL_CC_DEFINED
#define POSITIONTCL_CC_DEFINED

#include "position.h"
#include "poslist.h"

#include "io/iofactory.h"

#include "tcl/listitempkg.h"

#include <tcl.h>

#define NO_TRACE
#include "util/trace.h"
#include "util/debug.h"

namespace PosTcl {
  class TranslateCmd;
  class ScaleCmd;
  class RotateCmd;
  class PosPkg;
}

//---------------------------------------------------------------------
//
// TranslateCmd --
//
//---------------------------------------------------------------------

class PosTcl::TranslateCmd : public Tcl::TclItemCmd<Position> {
public:
  TranslateCmd(Tcl::TclItemPkg* pkg, const char* cmd_name) :
	 Tcl::TclItemCmd<Position>(pkg, cmd_name, 
								 "posid new_x new_y ?new_z?", 4, 5, false) {}
protected:
  virtual void invoke() {
	 Position* p = getItem();

	 double new_x=0.0, new_y=0.0, new_z=0.0;
	 if (objc() >= 5) { new_z = getDoubleFromArg(4); }
	 if (objc() >= 4) { new_y = getDoubleFromArg(3); }
	 if (objc() >= 3) { new_x = getDoubleFromArg(2); }

	 p->setTranslate(new_x, new_y, new_z);
	 returnVoid();
  }
};

//---------------------------------------------------------------------
//
// ScaleCmd --
//
//---------------------------------------------------------------------

class PosTcl::ScaleCmd : public Tcl::TclItemCmd<Position> {
public:
  ScaleCmd(Tcl::TclItemPkg* pkg, const char* cmd_name) :
	 Tcl::TclItemCmd<Position>(pkg, cmd_name, 
								 "posid scale_x ?scale_y? ?scale_z?", 3, 5, false) {}
protected:
  virtual void invoke() {
	 Position* p = getItem();

	 double new_x=0.0, new_y=0.0, new_z=0.0;
	 if (objc() >= 5) { new_z = getDoubleFromArg(4); }
	 if (objc() >= 4) { new_y = getDoubleFromArg(3); }
	 if (objc() >= 3) { new_x = getDoubleFromArg(2); }

	 p->setScale(new_x, new_y, new_z);
	 returnVoid();
  }
};

//---------------------------------------------------------------------
//
// RotateCmd --
//
//---------------------------------------------------------------------

class PosTcl::RotateCmd : public Tcl::TclItemCmd<Position> {
public:
  RotateCmd(Tcl::TclItemPkg* pkg, const char* cmd_name) :
	 Tcl::TclItemCmd<Position>(pkg, cmd_name, 
								 "posid angle ?rot_axis_x rot_axis_y rot_axis_z?",
								 3, 6, true) {}
protected:
  virtual void invoke() {
	 Position* p = getItem();

	 double new_a = getDoubleFromArg(2);
	 if (objc() == 3) {
		p->setAngle(new_a);
	 }
	 else if (objc() == 6) {
		double new_x = getDoubleFromArg(3);
		double new_y = getDoubleFromArg(4);
		double new_z = getDoubleFromArg(5);

		p->setRotate(new_a, new_x, new_y, new_z);
	 }
	 returnVoid();
  }
};

//---------------------------------------------------------------------
//
// PosPkg class definition
//
//---------------------------------------------------------------------

class PosTcl::PosPkg : public Tcl::ListItemPkg<Position, PosList> {
public:
  PosPkg(Tcl_Interp* interp) :
	 Tcl::ListItemPkg<Position, PosList>(interp, PosList::thePosList(),
													 "Pos", "2.3")
  {
	 addCommand( new RotateCmd(this, "Pos::rotate") );
	 addCommand( new TranslateCmd(this, "Pos::translate") );
	 addCommand( new ScaleCmd(this, "Pos::scale") );

	 Tcl_Eval(interp,
				 "namespace eval Pos { proc position {} { return Pos } }\n"
				 "proc posType {id} { return [Position::type $id] }\n");
  }
};


extern "C"
int Pos_Init(Tcl_Interp* interp) {
DOTRACE("Pos_Init");

  Tcl::TclPkg* pkg = new PosTcl::PosPkg(interp);

  FactoryRegistrar<IO::IoObject, Position>::registerWith(IO::IoFactory::theOne());

  return pkg->initStatus();
}

static const char vcid_positiontcl_cc[] = "$Header$";
#endif // !POSITIONTCL_CC_DEFINED
