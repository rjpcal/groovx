///////////////////////////////////////////////////////////////////////
//
// housetcl.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Sep 13 15:14:19 1999
// written: Thu Sep 16 11:14:25 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef HOUSETCL_CC_DEFINED
#define HOUSETCL_CC_DEFINED

#include <tcl.h>

#include "house.h"
#include "iomgr.h"
#include "objlist.h"
#include "listitempkg.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

///////////////////////////////////////////////////////////////////////
//
// HousePkg class definition
//
///////////////////////////////////////////////////////////////////////

/*
  \(.*\) its\(.*\);\(.*\)

    declareCAttrib("\2", &House::get\2, &House::set\2);
 */

namespace HouseTcl {
  class HousePkg;
}

class HouseTcl::HousePkg : public ListItemPkg<House, ObjList> {
public:
  HousePkg(Tcl_Interp* interp) :
	 ListItemPkg<House, ObjList>(interp, ObjList::theObjList(), "House", "1.1")
  {
	 declareCAttrib("storyAspectRatio", &House::getStoryAspectRatio, &House::setStoryAspectRatio);
    declareCAttrib("numStories", &House::getNumStories, &House::setNumStories);

    declareCAttrib("doorPosition", &House::getDoorPosition, &House::setDoorPosition);
    declareCAttrib("doorWidth", &House::getDoorWidth, &House::setDoorWidth);
    declareCAttrib("doorHeight", &House::getDoorHeight, &House::setDoorHeight);
	 declareCAttrib("doorOrientation", &House::getDoorOrientation, &House::setDoorOrientation);

    declareCAttrib("numWindows", &House::getNumWindows, &House::setNumWindows);
    declareCAttrib("windowWidth", &House::getWindowWidth, &House::setWindowWidth);
    declareCAttrib("windowHeight", &House::getWindowHeight, &House::setWindowHeight);
    declareCAttrib("windowVertBars", &House::getWindowVertBars, &House::setWindowVertBars);
    declareCAttrib("windowHorizBars", &House::getWindowHorizBars, &House::setWindowHorizBars);

    declareCAttrib("roofShape", &House::getRoofShape, &House::setRoofShape);
    declareCAttrib("roofOverhang", &House::getRoofOverhang, &House::setRoofOverhang);
    declareCAttrib("roofHeight", &House::getRoofHeight, &House::setRoofHeight);
    declareCAttrib("roofColor", &House::getRoofColor, &House::setRoofColor);

    declareCAttrib("chimneyXPosition", &House::getChimneyXPosition, &House::setChimneyXPosition);
    declareCAttrib("chimneyYPosition", &House::getChimneyYPosition, &House::setChimneyYPosition);
    declareCAttrib("chimneyHeight", &House::getChimneyHeight, &House::setChimneyHeight);	 
    declareCAttrib("chimneyWidth", &House::getChimneyWidth, &House::setChimneyWidth);	 
  }
};

extern "C" Tcl_PackageInitProc House_Init;

int House_Init(Tcl_Interp* interp) {
DOTRACE("House_Init");

  new HouseTcl::HousePkg(interp);

  FactoryRegistrar<IO, House> registrar(IoFactory::theOne());

  return TCL_OK;
}

static const char vcid_housetcl_cc[] = "$Header$";
#endif // !HOUSETCL_CC_DEFINED
