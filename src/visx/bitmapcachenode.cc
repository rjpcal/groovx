///////////////////////////////////////////////////////////////////////
//
// grobjrenderer.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Jul 19 11:22:10 2001
// written: Fri Aug 10 14:41:28 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GROBJRENDERER_CC_DEFINED
#define GROBJRENDERER_CC_DEFINED

#include "grobjrenderer.h"

#include "bitmaprep.h"
#include "glbmaprenderer.h"
#include "grobjimpl.h"
#include "xbmaprenderer.h"

#include "gfx/canvas.h"

#include "util/error.h"
#include "util/janitor.h"

#include <GL/gl.h>

#include "util/debug.h"
#include "util/trace.h"

fstring GrObjRenderer::BITMAP_CACHE_DIR(".");

GrObjRenderer::GrObjRenderer() :
#ifndef I686
  itsMode(Gmodes::GLCOMPILE),
#else
  itsMode(Gmodes::DIRECT_RENDER),
#endif
  itsUnMode(Gmodes::SWAP_FORE_BACK),
  itsCacheFilename(""),
  itsBitmapCache(0)
{
DOTRACE("GrObjRenderer::GrObjRenderer");
}

GrObjRenderer::~GrObjRenderer()
{
DOTRACE("GrObjRenderer::~GrObjRenderer");
}

void GrObjRenderer::setCacheFilename(const fstring& name)
{
DOTRACE("GrObjRenderer::setCacheFilename");
  itsCacheFilename = name;
  itsBitmapCache.reset( 0 );
}

void GrObjRenderer::invalidate()
{
DOTRACE("GrObjRenderer::invalidate");
  itsBitmapCache.reset( 0 );
}

bool GrObjRenderer::recacheBitmapIfNeeded(const Gnode* node,
														const GrObjImpl* obj,
                                          Gfx::Canvas& canvas) const
{
DOTRACE("GrObjRenderer::recacheBitmapIfNeeded");

  if (itsBitmapCache.get() != 0) return false;

  switch (itsMode)
    {
    case Gmodes::GL_BITMAP_CACHE:
      itsBitmapCache.reset(
         new BitmapRep(shared_ptr<BmapRenderer>(new GLBmapRenderer())));
      break;

    case Gmodes::X11_BITMAP_CACHE:
      itsBitmapCache.reset(
         new BitmapRep(shared_ptr<BmapRenderer>(new XBmapRenderer())));
      break;
    }

  Assert(itsBitmapCache.get() != 0);

  if ( !itsCacheFilename.empty() )
    {
      itsBitmapCache->queuePbmFile(fullCacheFilename().c_str());
      return false;
    }

  node->gnodeUndraw(canvas);

  Rect<double> bmapbox = node->gnodeBoundingBox(canvas);

  {
    Gfx::Canvas::StateSaver state(canvas);

    glPushAttrib(GL_COLOR_BUFFER_BIT);
    {
      glDrawBuffer(GL_FRONT);

      obj->itsScaler.doScaling(canvas);
      obj->itsAligner.doAlignment(canvas, bmapbox);

      node->gnodeDraw(canvas);

      itsBitmapCache->grabWorldRect(bmapbox);
    }
    glPopAttrib();
  }

  DebugEvalNL(itsMode);

  if (Gmodes::X11_BITMAP_CACHE == itsMode)
    {
      itsBitmapCache->flipVertical();
      itsBitmapCache->flipContrast();
    }

  return true;
}

void GrObjRenderer::setMode(Gmodes::RenderMode new_mode)
{
DOTRACE("GrObjRenderer::setMode");

#ifdef I686
  // display lists don't work at present with i686/linux/mesa
  if (new_mode == Gmodes::GLCOMPILE) new_mode = Gmodes::DIRECT_RENDER;
#endif

  if (new_mode != itsMode)
    invalidate();

  itsMode = new_mode;
}

void GrObjRenderer::render(const Gnode* node, const GrObjImpl* impl,
									Gfx::Canvas& canvas) const
{
DOTRACE("GrObjRenderer::render");
  DebugEvalNL(itsMode);
  switch (itsMode)
    {
    case Gmodes::GL_BITMAP_CACHE:
    case Gmodes::X11_BITMAP_CACHE:
		{
		  bool objectDrawn = recacheBitmapIfNeeded(node, impl, canvas);
		  if (!objectDrawn)
			 {
				Assert(itsBitmapCache.get() != 0);
				itsBitmapCache->render(canvas);
			 }
		}
      break;

	 default:
		node->gnodeDraw(canvas);
    }
}

void GrObjRenderer::unrender(const Gnode* node, Gfx::Canvas& canvas) const
{
DOTRACE("GrObjRenderer::unrender");

  node->gnodeUndraw(canvas);
}

void GrObjRenderer::saveBitmapCache(const Gnode* node, const GrObjImpl* obj,
												Gfx::Canvas& canvas,
                                    const char* filename) const
{
  itsCacheFilename = filename;

  GrObjRenderer temp;
  temp.setMode(Gmodes::GL_BITMAP_CACHE);
  temp.recacheBitmapIfNeeded(node, obj, canvas);

  temp.itsBitmapCache->savePbmFile(fullCacheFilename().c_str());

  itsCacheFilename = filename;
}

static const char vcid_grobjrenderer_cc[] = "$Header$";
#endif // !GROBJRENDERER_CC_DEFINED
