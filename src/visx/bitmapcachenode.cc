///////////////////////////////////////////////////////////////////////
//
// grobjrenderer.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Jul 19 11:22:10 2001
// written: Fri Aug 10 10:46:50 2001
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
#define DYNAMIC_TRACE_EXPR GrObj::tracer.status()
#include "util/trace.h"

fstring GrObjRenderer::BITMAP_CACHE_DIR(".");

GrObjRenderer::GrObjRenderer() :
#ifndef I686
  itsMode(GrObj::GLCOMPILE),
#else
  itsMode(GrObj::DIRECT_RENDER),
#endif
  itsUnMode(GrObj::SWAP_FORE_BACK),
  itsCacheFilename(""),
  itsIsCurrent(false),
  itsDisplayList(-1),
  itsBmapRenderer(0),
  itsBitmapCache(0)
{
DOTRACE("GrObjRenderer::GrObjRenderer");
}

GrObjRenderer::~GrObjRenderer() {
DOTRACE("GrObjRenderer::~GrObjRenderer");
  glDeleteLists(itsDisplayList, 1);
}

void GrObjRenderer::recompileIfNeeded(const GrObjImpl* obj,
                                      Gfx::Canvas& canvas) const {
DOTRACE("GrObjRenderer::recompileIfNeeded");
  if (itsIsCurrent) return;

  newList();

  glNewList(itsDisplayList, GL_COMPILE);
  obj->grRender(canvas, GrObj::DRAW);
  glEndList();

  postUpdated();
}

bool GrObjRenderer::recacheBitmapIfNeeded(const GrObjImpl* obj,
                                          Gfx::Canvas& canvas) const {
DOTRACE("GrObjRenderer::recacheBitmapIfNeeded");
  if (itsIsCurrent) return false;

  Assert(itsBitmapCache.get() != 0);

  obj->undraw(canvas);

  {
    Gfx::Canvas::StateSaver state(canvas);

    glPushAttrib(GL_COLOR_BUFFER_BIT|GL_PIXEL_MODE_BIT);
    {
      glDrawBuffer(GL_FRONT);

      obj->itsScaler.doScaling(canvas);
      obj->itsAligner.doAlignment(canvas, obj->grGetBoundingBox());

      obj->grRender(canvas, GrObj::DRAW);

      glReadBuffer(GL_FRONT);
      Rect<double> bmapbox_init = obj->grGetBoundingBox();
      Rect<int> screen_rect = canvas.getScreenFromWorld(bmapbox_init);
      screen_rect.widenByStep(2);
      screen_rect.heightenByStep(2);
      Rect<double> bmapbox = canvas.getWorldFromScreen(screen_rect);

      DebugEval(bmapbox.left()); DebugEval(bmapbox.top());
      DebugEval(bmapbox.right()); DebugEvalNL(bmapbox.bottom());
      itsBitmapCache->grabWorldRect(bmapbox);
      itsBitmapCache->setRasterPos(bmapbox.bottomLeft());
    }
    glPopAttrib();
  }

  DebugEvalNL(itsMode);

  if (GrObj::X11_BITMAP_CACHE == itsMode)
    {
      itsBitmapCache->flipVertical();
      itsBitmapCache->flipContrast();
    }

  if (GrObj::GL_BITMAP_CACHE == itsMode)
    {
      itsBitmapCache->flipContrast();
    }

  postUpdated();

  return true;
}

void GrObjRenderer::callList() const {
DOTRACE("GrObjRenderer::callList");
  // We must explicitly check that the display list is valid,
  // since it might be invalid if the object was recently
  // constructed, for example.
  if (glIsList(itsDisplayList) == GL_TRUE) {
    glCallList(itsDisplayList);
    DebugEvalNL(itsDisplayList);
  }
}

void GrObjRenderer::newList() const {
DOTRACE("GrObjRenderer::newList");
  glDeleteLists(itsDisplayList, 1);
  itsDisplayList = glGenLists(1);
  if (itsDisplayList == 0)
    {
      throw Util::Error("GrObj::newList: couldn't allocate display list");
    }
}

void GrObjRenderer::setMode(GrObj::RenderMode new_mode) {
DOTRACE("GrObjRenderer::setMode");

#ifdef I686
  // display lists don't work at present with i686/linux/mesa
  if (new_mode == GrObj::GLCOMPILE) new_mode = GrObj::DIRECT_RENDER;
#endif

  // If new_mode is the same as the current render mode, then return
  // immediately (and don't send a state change message)
  if (new_mode == itsMode) return;

  itsMode = new_mode;

  switch (new_mode)
    {
    case GrObj::GL_BITMAP_CACHE:
      itsBmapRenderer.reset(new GLBmapRenderer());
      itsBitmapCache.reset(new BitmapRep(itsBmapRenderer));
      queueBitmapLoad();
      break;

    case GrObj::X11_BITMAP_CACHE:
      itsBmapRenderer.reset(new XBmapRenderer());
      itsBitmapCache.reset(new BitmapRep(itsBmapRenderer));
      queueBitmapLoad();
      break;
    }
}

void GrObjRenderer::render(const GrObjImpl* obj, Gfx::Canvas& canvas) const {
DOTRACE("GrObjRenderer::render");
  DebugEvalNL(itsMode);
  switch (itsMode) {

  case GrObj::DIRECT_RENDER:
    obj->grRender(canvas, GrObj::DRAW);
    break;

  case GrObj::GLCOMPILE:
    callList();
    break;

  case GrObj::GL_BITMAP_CACHE:
  case GrObj::X11_BITMAP_CACHE:
    Assert(itsBitmapCache.get() != 0);
    itsBitmapCache->render(canvas);
    break;

  } // end switch
}

void GrObjRenderer::unrender(const GrObjImpl* obj, Gfx::Canvas& canvas) const
{
DOTRACE("GrObjRenderer::unrender");
  glPushAttrib(GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT);
  {
    if (itsUnMode == GrObj::SWAP_FORE_BACK)
      canvas.swapForeBack();

    if (itsMode == GrObj::GLCOMPILE)
      callList();
    else
      obj->grRender(canvas, GrObj::UNDRAW);
  }
  glPopAttrib();
}

bool GrObjRenderer::update(const GrObjImpl* obj, Gfx::Canvas& canvas) const {
DOTRACE("GrObjRenderer::update");
  canvas.throwIfError("before GrObj::update");

  bool objectDrawn = false;

  switch (itsMode) {
  case GrObj::GLCOMPILE:
    recompileIfNeeded(obj, canvas);
    break;

  case GrObj::GL_BITMAP_CACHE:
  case GrObj::X11_BITMAP_CACHE:
    objectDrawn = recacheBitmapIfNeeded(obj, canvas);
    break;
  }
  canvas.throwIfError("during GrObj::update");

  return objectDrawn;
}

void GrObjRenderer::saveBitmapCache(const GrObjImpl* obj, Gfx::Canvas& canvas,
                                    const char* filename)
{
  Util::Janitor<GrObjRenderer, int> rmj(*this,
                                        &GrObjRenderer::getMode,
                                        &GrObjRenderer::setMode);

  setMode(GrObj::X11_BITMAP_CACHE);

  setCacheFilename("");

  recacheBitmapIfNeeded(obj, canvas);
  itsCacheFilename = filename;

  itsBitmapCache->savePbmFile(fullCacheFilename().c_str());

  invalidate();
}

void GrObjRenderer::queueBitmapLoad() const
{
  if ( !itsCacheFilename.empty() && itsBitmapCache.get() != 0 )
    itsBitmapCache->queuePbmFile(fullCacheFilename().c_str());
}

static const char vcid_grobjrenderer_cc[] = "$Header$";
#endif // !GROBJRENDERER_CC_DEFINED
