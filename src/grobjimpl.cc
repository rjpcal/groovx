///////////////////////////////////////////////////////////////////////
//
// grobjimpl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Mar 23 16:27:57 2000
// written: Wed Jul 18 18:19:16 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GROBJIMPL_CC_DEFINED
#define GROBJIMPL_CC_DEFINED

#include "grobjimpl.h"

#include "io/reader.h"
#include "io/writer.h"

#include "gwt/canvas.h"

#include "util/error.h"
#include "util/janitor.h"

#include <GL/gl.h>
#include <GL/glu.h>

#define DYNAMIC_TRACE_EXPR GrObj::tracer.status()
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

class GrObjError : public ErrorWithMsg {
public:
  GrObjError(const char* msg) : ErrorWithMsg(msg) {}
};

///////////////////////////////////////////////////////////////////////
//
// File scope stuff
//
///////////////////////////////////////////////////////////////////////

const IO::VersionId GrObj::Impl::GROBJ_SERIAL_VERSION_ID;

dynamic_string GrObj::Impl::Renderer::BITMAP_CACHE_DIR(".");

namespace {

  GLenum BITMAP_CACHE_BUFFER = GL_FRONT;

#ifdef LOCAL_DEBUG
  inline void checkForGlError(const char* where) throw (GrObjError) {
    GLenum status = glGetError();
    if (status != GL_NO_ERROR) {
      const char* msg =
        reinterpret_cast<const char*>(gluErrorString(status));
      GrObjError err("GL error: ");
      err.appendMsg(msg);
      err.appendMsg(" ");
      err.appendMsg(where);
      throw err;
    }
  }
#else
#  define checkForGlError(x) {}
#endif
}


///////////////////////////////////////////////////////////////////////
//
// GrObj::Impl::BoundingBox definitions
//
///////////////////////////////////////////////////////////////////////

void GrObj::Impl::BoundingBox::updateRaw() const {
DOTRACE("GrObj::Impl::BoundingBox::updateRaw");
  DebugEval(itsRawBBIsCurrent);
  if (!itsRawBBIsCurrent)
    {
      itsOwner->grGetBoundingBox(itsCachedRawBB, itsCachedPixelBorder);
      itsRawBBIsCurrent = true;
    }
}

Rect<double> GrObj::Impl::BoundingBox::withBorder(
  const GWT::Canvas& canvas) const
{
DOTRACE("GrObj::Impl::BoundingBox::withBorder");

  // Do the object's internal scaling and alignment, and find the
  // bounding box in screen coordinates

  Rect<int> screen_pos;

  {
    glMatrixMode(GL_MODELVIEW);

    GWT::Canvas::StateSaver state(canvas);

    itsOwner->itsScaler.doScaling();
    itsOwner->itsAligner.doAlignment(native());

    screen_pos = canvas.getScreenFromWorld(native());
  }

  // Add a pixel border around the edges of the image...
  int bp = pixelBorder();

  screen_pos.widenByStep(bp);
  screen_pos.heightenByStep(bp);

  // ... and project back to world coordinates
  return canvas.getWorldFromScreen(screen_pos);
}



///////////////////////////////////////////////////////////////////////
//
// GrObj::Impl::Renderer definitions
//
///////////////////////////////////////////////////////////////////////

GrObj::Impl::Renderer::Renderer() :
  itsMode(GrObj::GLCOMPILE),
  itsCacheFilename(""),
  itsIsCurrent(false),
  itsDisplayList(-1),
  itsBmapRenderer(0),
  itsBitmapCache(0)
{
DOTRACE("GrObj::Impl::Renderer::Renderer");
}

GrObj::Impl::Renderer::~Renderer() {
DOTRACE("GrObj::Impl::Renderer::~Renderer");
  glDeleteLists(itsDisplayList, 1);
}

void GrObj::Impl::Renderer::recompileIfNeeded(const GrObj::Impl* obj,
                                              GWT::Canvas& canvas) const {
DOTRACE("GrObj::Impl::Renderer::recompileIfNeeded");
  if (itsIsCurrent) return;

  newList();

  glNewList(itsDisplayList, GLCOMPILE);
  obj->grRender(canvas, GrObj::DRAW);
  glEndList();

  postUpdated();
}

bool GrObj::Impl::Renderer::recacheBitmapIfNeeded(const GrObj::Impl* obj,
                                                  GWT::Canvas& canvas) const {
DOTRACE("GrObj::Impl::Renderer::recacheBitmapIfNeeded");
  if (itsIsCurrent) return false;

  Assert(itsBitmapCache.get() != 0);

  obj->undraw(canvas);

  {
    glMatrixMode(GL_MODELVIEW);
    GWT::Canvas::StateSaver state(canvas);

    glPushAttrib(GL_COLOR_BUFFER_BIT|GL_PIXEL_MODE_BIT);
    {
      glDrawBuffer(GL_FRONT);

      obj->itsScaler.doScaling();
      obj->itsAligner.doAlignment(obj->itsBB.native());

      obj->grRender(canvas, GrObj::DRAW);

      glReadBuffer(GL_FRONT);
      Rect<double> bmapbox_init = obj->getRawBB();
      Rect<int> screen_rect = canvas.getScreenFromWorld(bmapbox_init);
      screen_rect.widenByStep(2);
      screen_rect.heightenByStep(2);
      Rect<double> bmapbox = canvas.getWorldFromScreen(screen_rect);

      DebugEval(bmapbox.left()); DebugEval(bmapbox.top());
      DebugEval(bmapbox.right()); DebugEvalNL(bmapbox.bottom());
      itsBitmapCache->grabWorldRect(bmapbox);
      itsBitmapCache->setRasterX(bmapbox.left());
      itsBitmapCache->setRasterY(bmapbox.bottom());
    }
    glPopAttrib();
  }

  DebugEvalNL(itsMode);

  if (GrObj::X11_BITMAP_CACHE == itsMode) {
    itsBitmapCache->flipVertical();
    itsBitmapCache->flipContrast();
  }

  if (GrObj::GL_BITMAP_CACHE == itsMode) {
    itsBitmapCache->flipContrast();
  }

  postUpdated();

  return true;
}

void GrObj::Impl::Renderer::callList() const {
DOTRACE("GrObj::Impl::Renderer::callList");
  // We must explicitly check that the display list is valid,
  // since it might be invalid if the object was recently
  // constructed, for example.
  if (glIsList(itsDisplayList) == GL_TRUE) {
    glCallList(itsDisplayList);
    DebugEvalNL(itsDisplayList);
  }
}

void GrObj::Impl::Renderer::newList() const {
DOTRACE("GrObj::Impl::Renderer::newList");
  glDeleteLists(itsDisplayList, 1);
  itsDisplayList = glGenLists(1);
  if (itsDisplayList == 0) {
    throw GrObjError("GrObj::newList: couldn't allocate display list");
  }
}

void GrObj::Impl::Renderer::setMode(GrObj::RenderMode new_mode) {
DOTRACE("GrObj::Impl::Renderer::setMode");
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

void GrObj::Impl::Renderer::render(const GrObj::Impl* obj,
                                   GWT::Canvas& canvas) const {
DOTRACE("GrObj::Impl::Renderer::render");
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

bool GrObj::Impl::Renderer::update(const GrObj::Impl* obj,
                                   GWT::Canvas& canvas) const {
DOTRACE("GrObj::Impl::Renderer::update");
  checkForGlError("before GrObj::update");

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
  checkForGlError("during GrObj::update");

  return objectDrawn;
}

///////////////////////////////////////////////////////////////////////
//
// GrObj::Impl creator definitions
//
///////////////////////////////////////////////////////////////////////

GrObj::Impl::Impl(GrObj* obj) :
  self(obj),
  itsCategory(-1),
  itsBB(this),
  itsScaler(),
  itsAligner(),
  itsRenderer(),
  itsUnRenderer()
{};

GrObj::Impl::~Impl() {
DOTRACE("GrObj::Impl::~Impl");
}

void GrObj::Impl::readFrom(IO::Reader* reader) {
DOTRACE("GrObj::Impl::readFrom");

  reader->ensureReadVersionId("GrObj", 1, "Try grsh0.8a4");

  reader->readValue("GrObj::category", itsCategory);

  int temp;
  reader->readValue("GrObj::renderMode", temp);
  itsRenderer.setMode(temp);

  dynamic_string filename;
  reader->readValue("GrObj::cacheFilename", filename);
  itsRenderer.setCacheFilename(filename);

  reader->readValue("GrObj::unRenderMode", itsUnRenderer.itsMode);

  reader->readValue("GrObj::bbVisibility", itsBB.itsIsVisible);

  reader->readValue("GrObj::scalingMode", temp);
  itsScaler.setMode(temp);

  reader->readValue("GrObj::widthFactor", itsScaler.itsWidthFactor);
  reader->readValue("GrObj::heightFactor", itsScaler.itsHeightFactor);

  reader->readValue("GrObj::alignmentMode", itsAligner.itsMode);
  reader->readValue("GrObj::centerX", itsAligner.itsCenter.x());
  reader->readValue("GrObj::centerY", itsAligner.itsCenter.y());

  invalidateCaches();
}

void GrObj::Impl::writeTo(IO::Writer* writer) const {
DOTRACE("GrObj::Impl::writeTo");

  writer->ensureWriteVersionId("GrObj", GROBJ_SERIAL_VERSION_ID, 1,
                               "Try grsh0.8a4");

  writer->writeValue("GrObj::category", itsCategory);

  writer->writeValue("GrObj::renderMode", itsRenderer.getMode());

  writer->writeValue("GrObj::cacheFilename", itsRenderer.getCacheFilename());

  writer->writeValue("GrObj::unRenderMode", itsUnRenderer.itsMode);

  writer->writeValue("GrObj::bbVisibility", itsBB.itsIsVisible);

  writer->writeValue("GrObj::scalingMode", itsScaler.getMode());
  writer->writeValue("GrObj::widthFactor", itsScaler.itsWidthFactor);
  writer->writeValue("GrObj::heightFactor", itsScaler.itsHeightFactor);

  writer->writeValue("GrObj::alignmentMode", itsAligner.itsMode);
  writer->writeValue("GrObj::centerX", itsAligner.itsCenter.x());
  writer->writeValue("GrObj::centerY", itsAligner.itsCenter.y());
}


///////////////////////////////////////////////////////////////////////
//
// GrObj::Impl action definitions
//
///////////////////////////////////////////////////////////////////////

void GrObj::Impl::draw(GWT::Canvas& canvas) const {
DOTRACE("GrObj::Impl::draw");
  checkForGlError("before GrObj::draw");

  if ( itsBB.itsIsVisible ) {
    grDrawBoundingBox(canvas);
  }

  bool objectDrawn = itsRenderer.update(this, canvas);

  if ( !objectDrawn ) {

    {
      glMatrixMode(GL_MODELVIEW);

      GWT::Canvas::StateSaver state(canvas);

      itsScaler.doScaling();
      itsAligner.doAlignment(itsBB.native());

      itsRenderer.render(this, canvas);
    }
  }

  checkForGlError("during GrObj::draw");
}

void GrObj::Impl::saveBitmapCache(
  GWT::Canvas& canvas, const char* filename
  ) {
DOTRACE("GrObj::Impl::Renderer::saveBitmapCache");

  Util::Janitor<Impl, int> rmj(*this, &Impl::getRenderMode,
                               &Impl::setRenderMode);
  Util::Janitor<Impl, bool> bbj(*this, &Impl::getBBVisibility,
                                &Impl::setBBVisibility);

  setRenderMode(GrObj::X11_BITMAP_CACHE);
  setBBVisibility(false);

  itsRenderer.setCacheFilename("");

  itsRenderer.saveBitmapCache(this, canvas, filename);

  invalidateCaches();
}

void GrObj::Impl::undraw(GWT::Canvas& canvas) const {
DOTRACE("GrObj::Impl::undraw");
  checkForGlError("before GrObj::undraw");

  switch (itsUnRenderer.itsMode) {
  case GrObj::DIRECT_RENDER:     undrawDirectRender(canvas);     break;
  case GrObj::SWAP_FORE_BACK:    undrawSwapForeBack(canvas);     break;
  case GrObj::CLEAR_BOUNDING_BOX:undrawClearBoundingBox(canvas); break;
  default:                       /* nothing */                   break;
  }

  if ( itsBB.itsIsVisible ) {
    undrawBoundingBox(canvas);
  }

  checkForGlError("during GrObj::undraw");
}

void GrObj::Impl::grDrawBoundingBox(const GWT::Canvas& canvas) const {
DOTRACE("GrObj::Impl::grDrawBoundingBox");

  Rect<double> bbox = itsBB.withBorder(canvas);

  glPushAttrib(GL_LINE_BIT);
  {
    glLineWidth(1.0);
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(1, 0x0F0F);

    glBegin(GL_LINE_LOOP);
      glVertex2d(bbox.left(), bbox.bottom());
      glVertex2d(bbox.right(), bbox.bottom());
      glVertex2d(bbox.right(), bbox.top());
      glVertex2d(bbox.left(), bbox.top());
    glEnd();
  }
  glPopAttrib();
}

void GrObj::Impl::invalidateCaches() {
DOTRACE("GrObj::Impl::invalidateCaches");
  itsRenderer.invalidate();
  itsBB.invalidate();
}

void GrObj::Impl::undrawDirectRender(GWT::Canvas& canvas) const {
DOTRACE("GrObj::Impl::undrawDirectRender");
  glMatrixMode(GL_MODELVIEW);

  GWT::Canvas::StateSaver state(canvas);

  itsScaler.doScaling();
  itsAligner.doAlignment(itsBB.native());

  self->grRender(canvas, GrObj::UNDRAW);
}

void GrObj::Impl::undrawSwapForeBack(GWT::Canvas& canvas) const {
DOTRACE("GrObj::Impl::undrawSwapForeBack");
  glPushAttrib(GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT);
  {
    canvas.swapForeBack();

    {
      glMatrixMode(GL_MODELVIEW);

      GWT::Canvas::StateSaver state(canvas);

      itsScaler.doScaling();
      itsAligner.doAlignment(itsBB.native());

      if ( itsRenderer.getMode() == GrObj::GLCOMPILE ) {
        itsRenderer.callList();
      }
      else {
        self->grRender(canvas, GrObj::UNDRAW);
      }
    }
  }
  glPopAttrib();
}

void GrObj::Impl::undrawClearBoundingBox(GWT::Canvas& canvas) const {
DOTRACE("GrObj::Impl::undrawClearBoundingBox");
  glMatrixMode(GL_MODELVIEW);

  Rect<double> world_pos = itsBB.withBorder(canvas);

  glPushAttrib(GL_SCISSOR_BIT);
  {
    glEnable(GL_SCISSOR_TEST);

    Rect<int> screen_pos = canvas.getScreenFromWorld(world_pos);

    // Add an extra one-pixel border around the rect
    screen_pos.widenByStep(itsBB.pixelBorder());
    screen_pos.heightenByStep(itsBB.pixelBorder());

    glScissor(screen_pos.left(), screen_pos.bottom(),
              screen_pos.width(), screen_pos.height());

    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);
  }
  glPopAttrib();
}

void GrObj::Impl::undrawBoundingBox(GWT::Canvas& canvas) const {
DOTRACE("GrObj::Impl::undrawBoundingBox");
  glPushAttrib(GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT);
  {
    canvas.swapForeBack();

    {
      glMatrixMode(GL_MODELVIEW);

      GWT::Canvas::StateSaver state(canvas);

      itsScaler.doScaling();
      itsAligner.doAlignment(itsBB.native());

      grDrawBoundingBox(canvas);
    }
  }
  glPopAttrib();
}

static const char vcid_grobjimpl_cc[] = "$Header$";
#endif // !GROBJIMPL_CC_DEFINED
