///////////////////////////////////////////////////////////////////////
//
// grobjtcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Jul  1 14:01:18 1999
// written: Thu Jul 12 16:29:49 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GROBJTCL_CC_DEFINED
#define GROBJTCL_CC_DEFINED

#include "application.h"
#include "grobj.h"
#include "rect.h"

#include "tcl/genericobjpkg.h"
#include "tcl/objfunctor.h"
#include "tcl/tclerror.h"
#include "tcl/tcllistobj.h"
#include "tcl/tracertcl.h"

namespace Tcl
{
  template <class T>
  struct Convert<Rect<T> >
  {
	 typedef T Type;
	 static Rect<T> fromTcl( Tcl_Obj* obj )
	 {
		Tcl::List listObj(obj);
		return Rect<T>(listObj.get(0, (T*)0), listObj.get(1, (T*)0),
							listObj.get(2, (T*)0), listObj.get(3, (T*)0));
	 }

	 static Tcl_Obj* toTcl( Rect<T> rect )
	 {
		Tcl::List listObj;
		listObj.append(rect.left());
		listObj.append(rect.top());
		listObj.append(rect.right());
		listObj.append(rect.bottom());
	 }
  };
}

///////////////////////////////////////////////////////////////////////
//
// Grobj Tcl package
//
///////////////////////////////////////////////////////////////////////

namespace GrobjTcl
{
  Rect<double> boundingBox(Util::Ref<GrObj> obj)
  {
    GWT::Canvas& canvas = Application::theApp().getCanvas();

	 Rect<double> bbox;
	 bool have_box = obj->getBoundingBox(canvas, bbox);

	 if (!have_box)
		throw Tcl::TclError("object did not define a bounding box");

	 return bbox;
  }

  void saveBitmapCache(Util::Ref<GrObj> obj, const char* filename)
  {
    GWT::Canvas& canvas = Application::theApp().getCanvas();
    obj->saveBitmapCache(canvas, filename);
  }

  void update(Util::Ref<GrObj> obj)
  {
    GWT::Canvas& canvas = Application::theApp().getCanvas();
    obj->update(canvas);
  }

  class GrObjPkg;
};

//---------------------------------------------------------------------
//
// GrObjPkg definitions --
//
//---------------------------------------------------------------------

class GrobjTcl::GrObjPkg : public Tcl::GenericObjPkg<GrObj> {
public:
  GrObjPkg(Tcl_Interp* interp) :
    Tcl::GenericObjPkg<GrObj>(interp, "GrObj", "$Revision$")
  {
    Tcl::addTracing(this, GrObj::tracer);

	 Tcl::defVec( this, &GrobjTcl::boundingBox,
					  "GrObj::boundingBox", "item_id(s)" );
	 Tcl::defVec( this, &GrobjTcl::saveBitmapCache,
					  "GrObj::saveBitmapCache", "item_id(s) filename(s)" );
    declareCAction("restoreBitmapCache", &GrObj::restoreBitmapCache);
	 Tcl::defVec( this, &GrobjTcl::update,
					  "GrObj::update", "item_id(s)" );

	 Tcl::def( this, &GrObj::setBitmapCacheDir,
				  "GrObj::setBitmapCacheDir", "filename" );

    declareCAttrib("alignmentMode",
                   &GrObj::getAlignmentMode, &GrObj::setAlignmentMode);
    declareCAttrib("aspectRatio",
                   &GrObj::getAspectRatio, &GrObj::setAspectRatio);
    declareCAttrib("bbVisibility",
                   &GrObj::getBBVisibility, &GrObj::setBBVisibility);
    declareCAttrib("category", &GrObj::category, &GrObj::setCategory);
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

    linkVarCopy("GrObj::DIRECT_RENDER", GrObj::DIRECT_RENDER);
    linkVarCopy("GrObj::GLCOMPILE", GrObj::GLCOMPILE);
    linkVarCopy("GrObj::GL_BITMAP_CACHE", GrObj::GL_BITMAP_CACHE);
    linkVarCopy("GrObj::X11_BITMAP_CACHE", GrObj::X11_BITMAP_CACHE);
    linkVarCopy("GrObj::SWAP_FORE_BACK", GrObj::SWAP_FORE_BACK);
    linkVarCopy("GrObj::CLEAR_BOUNDING_BOX", GrObj::CLEAR_BOUNDING_BOX);

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
int Grobj_Init(Tcl_Interp* interp)
{
  Tcl::TclPkg* pkg = new GrobjTcl::GrObjPkg(interp);

  return pkg->initStatus();
}

static const char vcid_grobjtcl_cc[] = "$Header$";
#endif // !GROBJTCL_CC_DEFINED
