///////////////////////////////////////////////////////////////////////
//
// grobjtcl.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Jul  1 14:01:18 1999
// written: Wed Mar 15 12:00:18 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GROBJTCL_CC_DEFINED
#define GROBJTCL_CC_DEFINED

#include "application.h"
#include "experiment.h"
#include "grobj.h"
#include "objlist.h"
#include "rect.h"

#include "tcl/listitempkg.h"
#include "tcl/tclveccmds.h"
#include "tcl/tracertcl.h"

///////////////////////////////////////////////////////////////////////
//
// Grobj Tcl package
//
///////////////////////////////////////////////////////////////////////

namespace GrobjTcl {
  class BoundingBoxCmd;
  class Updater;
  class GrObjPkg;
};

//---------------------------------------------------------------------
//
// BoundingBoxCmd --
//
//---------------------------------------------------------------------

class GrobjTcl::BoundingBoxCmd : public Tcl::TclItemCmd<GrObj> {
public:
  BoundingBoxCmd(Tcl::TclItemPkg* pkg, const char* cmd_name) :
	 Tcl::TclItemCmd<GrObj>(pkg, cmd_name, "objid", 2, 2) {}
protected:
  virtual void invoke() {
	 Canvas* canvas = Application::theApp().getExperiment()->getCanvas();

	 Rect<double> bbox;
	 bool have_box = getItem()->getBoundingBox(*canvas, bbox);

	 if (have_box) {
		lappendVal(bbox.left());
		lappendVal(bbox.top());
		lappendVal(bbox.right());
		lappendVal(bbox.bottom());
	 }
  }
};

//---------------------------------------------------------------------
//
// Updater --
//
//---------------------------------------------------------------------

class GrobjTcl::Updater : public Action {
public:
  virtual void action(void* item) {
	 GrObj* obj = static_cast<GrObj*>(item);
	 Canvas* canvas = Application::theApp().getExperiment()->getCanvas();
	 obj->update(*canvas);
  }
};

//---------------------------------------------------------------------
//
// GrObjPkg definitions --
//
//---------------------------------------------------------------------

class GrobjTcl::GrObjPkg : public Tcl::AbstractListItemPkg<GrObj, ObjList> {
public:
  GrObjPkg(Tcl_Interp* interp) :
	 Tcl::AbstractListItemPkg<GrObj, ObjList>(interp, ObjList::theObjList(),
															"GrObj", "$Revision$")
  {
	 Tcl::addTracing(this, GrObj::tracer);

	 addCommand( new BoundingBoxCmd(this, "GrObj::boundingBox") );
	 addCommand( new Tcl::VecActionCmd(this, "GrObj::update",
												  new Updater, "item_id(s)", 1) );

	 declareCAttrib("alignmentMode",
						 &GrObj::getAlignmentMode, &GrObj::setAlignmentMode);
	 declareCAttrib("aspectRatio",
						 &GrObj::getAspectRatio, &GrObj::setAspectRatio);
	 declareCAttrib("bbVisibility",
						 &GrObj::getBBVisibility, &GrObj::setBBVisibility);
	 declareCAttrib("category", &GrObj::getCategory, &GrObj::setCategory);
	 declareCAttrib("centerX", &GrObj::getCenterX, &GrObj::setCenterX);
	 declareCAttrib("centerY", &GrObj::getCenterY, &GrObj::setCenterY);
	 declareCAttrib("height", &GrObj::getHeight, &GrObj::setHeight);
	 declareCAttrib("maxDimension",
						 &GrObj::getMaxDimension, &GrObj::setMaxDimension);
	 declareCAttrib("renderMode",
						 &GrObj::getRenderMode, &GrObj::setRenderMode);
	 declareCAttrib("scalingMode",
						 &GrObj::getScalingMode, &GrObj::setScalingMode);
	 declareCAttrib("unRenderMode",
						 &GrObj::getUnRenderMode, &GrObj::setUnRenderMode);
	 declareCAttrib("width", &GrObj::getWidth, &GrObj::setWidth);

	 linkVarCopy("GrObj::GROBJ_DIRECT_RENDER", GrObj::GROBJ_DIRECT_RENDER);
	 linkVarCopy("GrObj::GROBJ_GL_COMPILE", GrObj::GROBJ_GL_COMPILE);
	 linkVarCopy("GrObj::GROBJ_GL_BITMAP_CACHE", GrObj::GROBJ_GL_BITMAP_CACHE);
	 linkVarCopy("GrObj::GROBJ_X11_BITMAP_CACHE", GrObj::GROBJ_X11_BITMAP_CACHE);
	 linkVarCopy("GrObj::GROBJ_SWAP_FORE_BACK", GrObj::GROBJ_SWAP_FORE_BACK);
	 linkVarCopy("GrObj::GROBJ_CLEAR_BOUNDING_BOX", GrObj::GROBJ_CLEAR_BOUNDING_BOX);

	 linkVarCopy("GrObj::NATIVE_SCALING", GrObj::NATIVE_SCALING);
	 linkVarCopy("GrObj::MAINTAIN_ASPECT_SCALING", GrObj::MAINTAIN_ASPECT_SCALING);
	 linkVarCopy("GrObj::FREE_SCALING", GrObj::FREE_SCALING);

	 linkVarCopy("GrObj::NATIVE_ALIGNMENT", GrObj::NATIVE_ALIGNMENT);
	 linkVarCopy("GrObj::CENTER_ON_CENTER", GrObj::CENTER_ON_CENTER);
	 linkVarCopy("GrObj::NW_ON_CENTER", GrObj::NW_ON_CENTER);
	 linkVarCopy("GrObj::NE_ON_CENTER", GrObj::NE_ON_CENTER);
	 linkVarCopy("GrObj::SW_ON_CENTER", GrObj::SW_ON_CENTER);
	 linkVarCopy("GrObj::SE_ON_CENTER", GrObj::SE_ON_CENTER);
	 linkVarCopy("GrObj::ARBITRARY_ON_CENTER", GrObj::ARBITRARY_ON_CENTER);
  }
};

//---------------------------------------------------------------------
//
// Grobj_Init --
//
//---------------------------------------------------------------------

extern "C"
int Grobj_Init(Tcl_Interp* interp) {

  Tcl::TclPkg* pkg = new GrobjTcl::GrObjPkg(interp);

  return pkg->initStatus();
}

static const char vcid_grobjtcl_cc[] = "$Header$";
#endif // !GROBJTCL_CC_DEFINED
