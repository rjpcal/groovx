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
  itsDisplayList(0),
  itsBitmapCache(0)
{
DOTRACE("GrObjRenderer::GrObjRenderer");
}

GrObjRenderer::~GrObjRenderer()
{
DOTRACE("GrObjRenderer::~GrObjRenderer");
  glDeleteLists(itsDisplayList, 1);
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
  glDeleteLists(itsDisplayList, 1);
  itsDisplayList = 0;

  itsBitmapCache.reset( 0 );
}

void GrObjRenderer::recompileIfNeeded(const Gnode* node,
                                      Gfx::Canvas& canvas) const
{
DOTRACE("GrObjRenderer::recompileIfNeeded");
  if (itsDisplayList == 0)
    {
      itsDisplayList = glGenLists(1);

      if (itsDisplayList == 0)
        {
          throw Util::Error("GrObj::newList: couldn't allocate display list");
        }

      glNewList(itsDisplayList, GL_COMPILE);
      node->gnodeDraw(canvas);
      glEndList();
    }
}

bool GrObjRenderer::recacheBitmapIfNeeded(const GrObjImpl* obj,
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

  obj->itsObjNode->gnodeUndraw(canvas);

  Rect<double> bmapbox = obj->itsObjNode->gnodeBoundingBox(canvas);

  {
    Gfx::Canvas::StateSaver state(canvas);

    glPushAttrib(GL_COLOR_BUFFER_BIT);
    {
      glDrawBuffer(GL_FRONT);

      obj->itsScaler.doScaling(canvas);
      obj->itsAligner.doAlignment(canvas, bmapbox);

      obj->itsObjNode->gnodeDraw(canvas);

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

void GrObjRenderer::callList() const
{
DOTRACE("GrObjRenderer::callList");
  // We must explicitly check that the display list is valid,
  // since it might be invalid if the object was recently
  // constructed, for example.
  if (glIsList(itsDisplayList) == GL_TRUE)
    {
      glCallList(itsDisplayList);
      DebugEvalNL(itsDisplayList);
    }
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
  switch (itsMode)
    {
    case Gmodes::DIRECT_RENDER:
      node->gnodeDraw(canvas);
      break;

    case Gmodes::GLCOMPILE:
      callList();
      break;

    case Gmodes::GL_BITMAP_CACHE:
    case Gmodes::X11_BITMAP_CACHE:
      Assert(itsBitmapCache.get() != 0);
      itsBitmapCache->render(canvas);
      break;
    }
}

void GrObjRenderer::unrender(const Gnode* node, Gfx::Canvas& canvas) const
{
DOTRACE("GrObjRenderer::unrender");
  glPushAttrib(GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT);
  {
    if (itsUnMode == Gmodes::SWAP_FORE_BACK)
      canvas.swapForeBack();

    if (itsMode == Gmodes::GLCOMPILE)
      callList();
    else
      node->gnodeUndraw(canvas);
  }
  glPopAttrib();
}

bool GrObjRenderer::update(const GrObjImpl* obj, Gfx::Canvas& canvas) const
{
DOTRACE("GrObjRenderer::update");
  canvas.throwIfError("before GrObj::update");

  bool objectDrawn = false;

  switch (itsMode)
    {
    case Gmodes::GLCOMPILE:
      recompileIfNeeded(obj->itsObjNode.get(), canvas);
      break;

    case Gmodes::GL_BITMAP_CACHE:
    case Gmodes::X11_BITMAP_CACHE:
      objectDrawn = recacheBitmapIfNeeded(obj, canvas);
      break;
    }
  canvas.throwIfError("during GrObj::update");

  return objectDrawn;
}

void GrObjRenderer::saveBitmapCache(const GrObjImpl* obj, Gfx::Canvas& canvas,
                                    const char* filename) const
{
  itsCacheFilename = filename;

  GrObjRenderer temp;
  temp.setMode(Gmodes::GL_BITMAP_CACHE);
  temp.recacheBitmapIfNeeded(obj, canvas);

  temp.itsBitmapCache->savePbmFile(fullCacheFilename().c_str());

  itsCacheFilename = filename;
}

static const char vcid_grobjrenderer_cc[] = "$Header$";
#endif // !GROBJRENDERER_CC_DEFINED
