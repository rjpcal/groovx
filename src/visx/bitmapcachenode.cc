///////////////////////////////////////////////////////////////////////
//
// grobjrenderer.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Jul 19 11:22:10 2001
// written: Fri Aug 10 18:10:20 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GROBJRENDERER_CC_DEFINED
#define GROBJRENDERER_CC_DEFINED

#include "grobjrenderer.h"

#include "bitmaprep.h"
#include "glbmaprenderer.h"
#include "rect.h"
#include "xbmaprenderer.h"

#include "gfx/canvas.h"

#include "util/error.h"

#include <GL/gl.h>

#include "util/debug.h"
#include "util/trace.h"

fstring GrObjRenderer::BITMAP_CACHE_DIR(".");

GrObjRenderer::GrObjRenderer() :
  itsMode(Gmodes::DIRECT_RENDER),
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

bool GrObjRenderer::recacheBitmap(const Gnode* node, Gfx::Canvas& canvas) const
{
DOTRACE("GrObjRenderer::recacheBitmap");

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

  glPushAttrib(GL_COLOR_BUFFER_BIT);
  {
    glDrawBuffer(GL_FRONT);

    node->gnodeDraw(canvas);

    itsBitmapCache->grabWorldRect(bmapbox);
  }
  glPopAttrib();

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

void GrObjRenderer::render(const Gnode* node, Gfx::Canvas& canvas) const
{
DOTRACE("GrObjRenderer::render");
  DebugEvalNL(itsMode);

  if (itsMode != Gmodes::GL_BITMAP_CACHE &&
      itsMode != Gmodes::X11_BITMAP_CACHE)
    {
      node->gnodeDraw(canvas);
    }
  else
    {
      bool objectDrawn = recacheBitmap(node, canvas);
      if (!objectDrawn)
        {
          Assert(itsBitmapCache.get() != 0);
          itsBitmapCache->render(canvas);
        }
    }
}

void GrObjRenderer::unrender(const Gnode* node, Gfx::Canvas& canvas) const
{
DOTRACE("GrObjRenderer::unrender");

  node->gnodeUndraw(canvas);
}

void GrObjRenderer::saveBitmapCache(const Gnode* node, Gfx::Canvas& canvas,
                                    const char* filename) const
{
  if (itsBitmapCache.get() != 0)
    {
      itsCacheFilename = filename;
      itsBitmapCache->savePbmFile(fullCacheFilename().c_str());
    }
}

static const char vcid_grobjrenderer_cc[] = "$Header$";
#endif // !GROBJRENDERER_CC_DEFINED
