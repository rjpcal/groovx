///////////////////////////////////////////////////////////////////////
//
// grobjtcl.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Jul  1 14:01:18 1999
// written: Thu Nov 14 19:01:44 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GROBJTCL_CC_DEFINED
#define GROBJTCL_CC_DEFINED

#include "gfx/canvas.h"
#include "gfx/gxaligner.h"
#include "gfx/gxcache.h"
#include "gfx/gxscaler.h"
#include "gfx/recttcl.h"

#include "gx/rect.h"

#include "tcl/fieldpkg.h"
#include "tcl/tclpkg.h"
#include "tcl/tracertcl.h"

#include "visx/grobj.h"

#include "util/trace.h"


namespace
{
  Gfx::Rect<double> boundingBox(Util::Ref<GrObj> obj)
  {
    return obj->getBoundingBox(Gfx::Canvas::current());
  }

  // This is gone for now because the bitmap cache node is gone from GrObj,
  // but we can resurrect a cleaner saveBitmap() function pretty easily,
  // just by captuing the screen bounds into a BmapData object and then
  // saving that.
#if 0
  void saveBitmap(Util::Ref<GrObj> obj, const char* filename)
  {
    obj->saveBitmapCache(Gfx::Canvas::current(), filename);
  }
#endif
}


extern "C"
int Grobj_Init(Tcl_Interp* interp)
{
DOTRACE("Grobj_Init");
  Tcl::Pkg* pkg = new Tcl::Pkg(interp, "GrObj", "$Revision$");

  Tcl::defTracing(pkg, GrObj::tracer);

  Tcl::defFieldContainer<GrObj>(pkg);

  pkg->defVec( "boundingBox", "item_id(s)", &::boundingBox );
#if 0
  pkg->defVec( "saveBitmap", "item_id(s) filename(s)", &::saveBitmap );
#endif

  pkg->defAttrib("category", &GrObj::category, &GrObj::setCategory);

  pkg->linkVarCopy("GrObj::DIRECT", GxCache::DIRECT);
  pkg->linkVarCopy("GrObj::GLCOMPILE", GxCache::GLCOMPILE);

  pkg->linkVarCopy("GrObj::NATIVE_SCALING", GxScaler::NATIVE_SCALING);
  pkg->linkVarCopy("GrObj::MAINTAIN_ASPECT_SCALING", GxScaler::MAINTAIN_ASPECT_SCALING);
  pkg->linkVarCopy("GrObj::FREE_SCALING", GxScaler::FREE_SCALING);

  pkg->linkVarCopy("GrObj::NATIVE_ALIGNMENT", GxAligner::NATIVE_ALIGNMENT);
  pkg->linkVarCopy("GrObj::CENTER_ON_CENTER", GxAligner::CENTER_ON_CENTER);
  pkg->linkVarCopy("GrObj::NW_ON_CENTER", GxAligner::NW_ON_CENTER);
  pkg->linkVarCopy("GrObj::NE_ON_CENTER", GxAligner::NE_ON_CENTER);
  pkg->linkVarCopy("GrObj::SW_ON_CENTER", GxAligner::SW_ON_CENTER);
  pkg->linkVarCopy("GrObj::SE_ON_CENTER", GxAligner::SE_ON_CENTER);
  pkg->linkVarCopy("GrObj::ARBITRARY_ON_CENTER", GxAligner::ARBITRARY_ON_CENTER);

  return pkg->initStatus();
}

static const char vcid_grobjtcl_cc[] = "$Header$";
#endif // !GROBJTCL_CC_DEFINED
