///////////////////////////////////////////////////////////////////////
//
// grobjtcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Jul  1 14:01:18 1999
// written: Wed Aug 15 13:37:53 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GROBJTCL_CC_DEFINED
#define GROBJTCL_CC_DEFINED

#include "application.h"
#include "grobj.h"
#include "recttcl.h"

#include "gfx/rect.h"

#include "tcl/fieldpkg.h"
#include "tcl/tclpkg.h"
#include "tcl/tracertcl.h"

///////////////////////////////////////////////////////////////////////
//
// Grobj Tcl package
//
///////////////////////////////////////////////////////////////////////

namespace GrobjTcl
{
  Gfx::Rect<double> boundingBox(Util::Ref<GrObj> obj)
  {
    Gfx::Canvas& canvas = Application::theApp().getCanvas();

    Gfx::Rect<double> bbox;
    obj->getBoundingBox(canvas, bbox);

    return bbox;
  }

  void saveBitmapCache(Util::Ref<GrObj> obj, const char* filename)
  {
    Gfx::Canvas& canvas = Application::theApp().getCanvas();
    obj->saveBitmapCache(canvas, filename);
  }

  void update(Util::Ref<GrObj> obj)
  {
    Gfx::Canvas& canvas = Application::theApp().getCanvas();
    obj->update(canvas);
  }

  class GrObjPkg;
};

//---------------------------------------------------------------------
//
// GrObjPkg definitions --
//
//---------------------------------------------------------------------

class GrobjTcl::GrObjPkg : public Tcl::Pkg {
public:
  GrObjPkg(Tcl_Interp* interp) :
    Tcl::Pkg(interp, "GrObj", "$Revision$")
  {
    Tcl::defTracing(this, GrObj::tracer);

    Tcl::defFieldContainer<GrObj>(this);

    defVec( "boundingBox", "item_id(s)", &GrobjTcl::boundingBox );
    defVec( "saveBitmapCache", "item_id(s) filename(s)",
            &GrobjTcl::saveBitmapCache );
    defVec( "update", "item_id(s)", &GrobjTcl::update );

    def( "setBitmapCacheDir", "filename", &GrObj::setBitmapCacheDir );

    defAttrib("category", &GrObj::category, &GrObj::setCategory);

    linkVarCopy("GrObj::DIRECT_RENDER", Gmodes::DIRECT_RENDER);
    linkVarCopy("GrObj::GLCOMPILE", Gmodes::GLCOMPILE);
    linkVarCopy("GrObj::GL_BITMAP_CACHE", Gmodes::GL_BITMAP_CACHE);
    linkVarCopy("GrObj::X11_BITMAP_CACHE", Gmodes::X11_BITMAP_CACHE);
    linkVarCopy("GrObj::SWAP_FORE_BACK", Gmodes::SWAP_FORE_BACK);
    linkVarCopy("GrObj::CLEAR_BOUNDING_BOX", Gmodes::CLEAR_BOUNDING_BOX);

    linkVarCopy("GrObj::NATIVE_SCALING", Gmodes::NATIVE_SCALING);
    linkVarCopy("GrObj::MAINTAIN_ASPECT_SCALING", Gmodes::MAINTAIN_ASPECT_SCALING);
    linkVarCopy("GrObj::FREE_SCALING", Gmodes::FREE_SCALING);

    linkVarCopy("GrObj::NATIVE_ALIGNMENT", Gmodes::NATIVE_ALIGNMENT);
    linkVarCopy("GrObj::CENTER_ON_CENTER", Gmodes::CENTER_ON_CENTER);
    linkVarCopy("GrObj::NW_ON_CENTER", Gmodes::NW_ON_CENTER);
    linkVarCopy("GrObj::NE_ON_CENTER", Gmodes::NE_ON_CENTER);
    linkVarCopy("GrObj::SW_ON_CENTER", Gmodes::SW_ON_CENTER);
    linkVarCopy("GrObj::SE_ON_CENTER", Gmodes::SE_ON_CENTER);
    linkVarCopy("GrObj::ARBITRARY_ON_CENTER", Gmodes::ARBITRARY_ON_CENTER);
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
  Tcl::Pkg* pkg = new GrobjTcl::GrObjPkg(interp);

  return pkg->initStatus();
}

static const char vcid_grobjtcl_cc[] = "$Header$";
#endif // !GROBJTCL_CC_DEFINED
