///////////////////////////////////////////////////////////////////////
//
// grobjtcl.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Jul  1 14:01:18 1999
// written: Wed Nov 13 12:53:16 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GROBJTCL_CC_DEFINED
#define GROBJTCL_CC_DEFINED

#include "grsh/grsh.h"

#include "gx/rect.h"

#include "tcl/fieldpkg.h"
#include "tcl/tclpkg.h"
#include "tcl/tracertcl.h"

#include "visx/gmodes.h"
#include "visx/grobj.h"
#include "visx/grobjaligner.h"
#include "visx/recttcl.h"

#include "util/trace.h"


namespace
{
  Gfx::Rect<double> boundingBox(Util::Ref<GrObj> obj)
  {
    return obj->getBoundingBox(Grsh::canvas());
  }

  void saveBitmapCache(Util::Ref<GrObj> obj, const char* filename)
  {
    obj->saveBitmapCache(Grsh::canvas(), filename);
  }

  void update(Util::Ref<GrObj> obj)
  {
    obj->update(Grsh::canvas());
  }
}


extern "C"
int Grobj_Init(Tcl_Interp* interp)
{
DOTRACE("Grobj_Init");
  Tcl::Pkg* pkg = new Tcl::Pkg(interp, "GrObj", "$Revision$");

  Tcl::defTracing(pkg, GrObj::tracer);

  Tcl::defFieldContainer<GrObj>(pkg);

  pkg->defVec( "boundingBox", "item_id(s)", &::boundingBox );
  pkg->defVec( "saveBitmapCache", "item_id(s) filename(s)", &::saveBitmapCache );
  pkg->defVec( "update", "item_id(s)", &::update );

  pkg->def( "setBitmapCacheDir", "filename", &GrObj::setBitmapCacheDir );

  pkg->defAttrib("category", &GrObj::category, &GrObj::setCategory);

  pkg->linkVarCopy("GrObj::DIRECT_RENDER", Gmodes::DIRECT_RENDER);
  pkg->linkVarCopy("GrObj::GLCOMPILE", Gmodes::GLCOMPILE);
  pkg->linkVarCopy("GrObj::GL_BITMAP_CACHE", Gmodes::GL_BITMAP_CACHE);
  pkg->linkVarCopy("GrObj::X11_BITMAP_CACHE", Gmodes::X11_BITMAP_CACHE);

  pkg->linkVarCopy("GrObj::NATIVE_SCALING", Gmodes::NATIVE_SCALING);
  pkg->linkVarCopy("GrObj::MAINTAIN_ASPECT_SCALING", Gmodes::MAINTAIN_ASPECT_SCALING);
  pkg->linkVarCopy("GrObj::FREE_SCALING", Gmodes::FREE_SCALING);

  pkg->linkVarCopy("GrObj::NATIVE_ALIGNMENT", GrObjAligner::NATIVE_ALIGNMENT);
  pkg->linkVarCopy("GrObj::CENTER_ON_CENTER", GrObjAligner::CENTER_ON_CENTER);
  pkg->linkVarCopy("GrObj::NW_ON_CENTER", GrObjAligner::NW_ON_CENTER);
  pkg->linkVarCopy("GrObj::NE_ON_CENTER", GrObjAligner::NE_ON_CENTER);
  pkg->linkVarCopy("GrObj::SW_ON_CENTER", GrObjAligner::SW_ON_CENTER);
  pkg->linkVarCopy("GrObj::SE_ON_CENTER", GrObjAligner::SE_ON_CENTER);
  pkg->linkVarCopy("GrObj::ARBITRARY_ON_CENTER", GrObjAligner::ARBITRARY_ON_CENTER);

  return pkg->initStatus();
}

static const char vcid_grobjtcl_cc[] = "$Header$";
#endif // !GROBJTCL_CC_DEFINED
