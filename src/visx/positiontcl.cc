///////////////////////////////////////////////////////////////////////
//
// positiontcl.cc
// Rob Peters
// created: Sat Mar 13 12:53:34 1999
// written: Mon Jul 19 15:25:20 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef POSITIONTCL_CC_DEFINED
#define POSITIONTCL_CC_DEFINED

#include "positiontcl.h"

#include <tcl.h>
#include <typeinfo>

#include "iomgr.h"
#include "position.h"
#include "poslist.h"
#include "listitempkg.h"
#include "tclcmd.h"

#define NO_TRACE
#include "trace.h"
#include "debug.h"

namespace PosTcl {
  class TranslateCmd;
  class ScaleCmd;
  class RotateCmd;
  class TypeCmd;
  class PosPkg;
}

//---------------------------------------------------------------------
//
// TranslateCmd --
//
//---------------------------------------------------------------------

class PosTcl::TranslateCmd : public TclItemCmd<Position> {
public:
  TranslateCmd(TclItemPkg* pkg, const char* cmd_name) :
	 TclItemCmd<Position>(pkg, cmd_name, 
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

class PosTcl::ScaleCmd : public TclItemCmd<Position> {
public:
  ScaleCmd(TclItemPkg* pkg, const char* cmd_name) :
	 TclItemCmd<Position>(pkg, cmd_name, 
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

class PosTcl::RotateCmd : public TclItemCmd<Position> {
public:
  RotateCmd(TclItemPkg* pkg, const char* cmd_name) :
	 TclItemCmd<Position>(pkg, cmd_name, 
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
// TypeCmd --
//
//---------------------------------------------------------------------

class PosTcl::TypeCmd : public TclItemCmd<Position> {
public:
  TypeCmd(TclItemPkg* pkg, const char* cmd_name) :
	 TclItemCmd<Position>(pkg, cmd_name, "posid", 2, 2) {}
protected:
  virtual void invoke() {
	 Position* p = getItem();
	 returnCstring(typeid(*p).name());
  }
};

//---------------------------------------------------------------------
//
// PosPkg class definition
//
//---------------------------------------------------------------------

class PosTcl::PosPkg : public ListItemPkg<Position, PosList> {
public:
  PosPkg(Tcl_Interp* interp) :
	 ListItemPkg<Position, PosList>(interp, PosList::thePosList(), "Pos", "2.3")
  {
	 addCommand( new RotateCmd(this, "Pos::rotate") );
	 addCommand( new TranslateCmd(this, "Pos::translate") );
	 addCommand( new ScaleCmd(this, "Pos::scale") );
	 addCommand( new TypeCmd(this, "Pos::type") );

	 Tcl_Eval(interp,
				 "namespace eval Pos { proc position {} { return Pos } }\n"
				 "proc posType {id} { return [Position::type $id] }\n");
  }
};


int Pos_Init(Tcl_Interp* interp) {
DOTRACE("Pos_Init");

  new PosTcl::PosPkg(interp);

  FactoryRegistrar<IO, Position> registrar(IoFactory::theOne());

  return TCL_OK;
}

static const char vcid_positiontcl_cc[] = "$Header$";
#endif // !POSITIONTCL_CC_DEFINED
