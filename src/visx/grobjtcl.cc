///////////////////////////////////////////////////////////////////////
//
// grobjtcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Jul  1 14:01:18 1999
// written: Sat Aug 25 21:49:00 2001
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

    obj->getBoundingBox(bbox, canvas);

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
};

//---------------------------------------------------------------------
//
// Grobj_Init --
//
//---------------------------------------------------------------------

extern "C"
int Grobj_Init(Tcl_Interp* interp)
{
  Tcl::Pkg* pkg = new Tcl::Pkg(interp, "GrObj", "$Revision$");

  Tcl::defTracing(pkg, GrObj::tracer);

  Tcl::defFieldContainer<GrObj>(pkg);

  pkg->defVec( "boundingBox", "item_id(s)", &GrobjTcl::boundingBox );
  pkg->defVec( "saveBitmapCache", "item_id(s) filename(s)",
               &GrobjTcl::saveBitmapCache );
  pkg->defVec( "update", "item_id(s)", &GrobjTcl::update );

  pkg->def( "setBitmapCacheDir", "filename", &GrObj::setBitmapCacheDir );

  pkg->defAttrib("category", &GrObj::category, &GrObj::setCategory);

  pkg->linkVarCopy("GrObj::DIRECT_RENDER", Gmodes::DIRECT_RENDER);
  pkg->linkVarCopy("GrObj::GLCOMPILE", Gmodes::GLCOMPILE);
  pkg->linkVarCopy("GrObj::GL_BITMAP_CACHE", Gmodes::GL_BITMAP_CACHE);
  pkg->linkVarCopy("GrObj::X11_BITMAP_CACHE", Gmodes::X11_BITMAP_CACHE);
  pkg->linkVarCopy("GrObj::CLEAR_BOUNDING_BOX", Gmodes::CLEAR_BOUNDING_BOX);

  pkg->linkVarCopy("GrObj::NATIVE_SCALING", Gmodes::NATIVE_SCALING);
  pkg->linkVarCopy("GrObj::MAINTAIN_ASPECT_SCALING", Gmodes::MAINTAIN_ASPECT_SCALING);
  pkg->linkVarCopy("GrObj::FREE_SCALING", Gmodes::FREE_SCALING);

  pkg->linkVarCopy("GrObj::NATIVE_ALIGNMENT", Gmodes::NATIVE_ALIGNMENT);
  pkg->linkVarCopy("GrObj::CENTER_ON_CENTER", Gmodes::CENTER_ON_CENTER);
  pkg->linkVarCopy("GrObj::NW_ON_CENTER", Gmodes::NW_ON_CENTER);
  pkg->linkVarCopy("GrObj::NE_ON_CENTER", Gmodes::NE_ON_CENTER);
  pkg->linkVarCopy("GrObj::SW_ON_CENTER", Gmodes::SW_ON_CENTER);
  pkg->linkVarCopy("GrObj::SE_ON_CENTER", Gmodes::SE_ON_CENTER);
  pkg->linkVarCopy("GrObj::ARBITRARY_ON_CENTER", Gmodes::ARBITRARY_ON_CENTER);

  return pkg->initStatus();
}

static const char vcid_grobjtcl_cc[] = "$Header$";
#endif // !GROBJTCL_CC_DEFINED
