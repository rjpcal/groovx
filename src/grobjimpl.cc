///////////////////////////////////////////////////////////////////////
//
// grobjimpl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Mar 23 16:27:57 2000
// written: Fri Aug 10 15:02:08 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GROBJIMPL_CC_DEFINED
#define GROBJIMPL_CC_DEFINED

#include "grobjimpl.h"

#include "gfx/canvas.h"

#include "io/reader.h"
#include "io/writer.h"

#include "util/error.h"

#define DYNAMIC_TRACE_EXPR GrObj::tracer.status()
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// File scope stuff
//
///////////////////////////////////////////////////////////////////////

namespace
{
  const IO::VersionId GROBJ_SERIAL_VERSION_ID = 1;
}


///////////////////////////////////////////////////////////////////////
//
// GrObjImpl creator definitions
//
///////////////////////////////////////////////////////////////////////

GrObjImpl::GrObjImpl(GrObj* obj) :
  itsObjNode(new GrObjNode(obj)),
  itsCategory(-1),
  itsBB(this),
  itsScaler(),
  itsAligner(),
  itsRenderer()
{};

GrObjImpl::~GrObjImpl()
{
DOTRACE("GrObjImpl::~GrObjImpl");
}

IO::VersionId GrObjImpl::serialVersionId() const
{
  return GROBJ_SERIAL_VERSION_ID;
}

void GrObjImpl::readFrom(IO::Reader* reader)
{
DOTRACE("GrObjImpl::readFrom");

  reader->ensureReadVersionId("GrObj", 1, "Try grsh0.8a4");

  reader->readValue("GrObj::category", itsCategory);

  {
    int temp;
    reader->readValue("GrObj::renderMode", temp);
    itsRenderer.setMode(temp);
  }

  {
    fstring filename;
    reader->readValue("GrObj::cacheFilename", filename);
    itsRenderer.setCacheFilename(filename);
  }

  {
    int temp;
    reader->readValue("GrObj::unRenderMode", temp);
    itsRenderer.setUnMode(temp);
  }

  {
    bool val;
    reader->readValue("GrObj::bbVisibility", val);
    itsBB.setVisible(val);
  }

  {
    int temp;
    reader->readValue("GrObj::scalingMode", temp);
    itsScaler.setMode(temp);
  }

  reader->readValue("GrObj::widthFactor", itsScaler.itsWidthFactor);
  reader->readValue("GrObj::heightFactor", itsScaler.itsHeightFactor);

  reader->readValue("GrObj::alignmentMode", itsAligner.itsMode);
  reader->readValue("GrObj::centerX", itsAligner.itsCenter.x());
  reader->readValue("GrObj::centerY", itsAligner.itsCenter.y());

  invalidateCaches();
}

void GrObjImpl::writeTo(IO::Writer* writer) const
{
DOTRACE("GrObjImpl::writeTo");

  writer->ensureWriteVersionId("GrObj", GROBJ_SERIAL_VERSION_ID, 1,
                               "Try grsh0.8a4");

  writer->writeValue("GrObj::category", itsCategory);

  writer->writeValue("GrObj::renderMode", itsRenderer.getMode());

  writer->writeValue("GrObj::cacheFilename", itsRenderer.getCacheFilename());

  writer->writeValue("GrObj::unRenderMode", itsRenderer.getUnMode());

  writer->writeValue("GrObj::bbVisibility", itsBB.isVisible());

  writer->writeValue("GrObj::scalingMode", itsScaler.getMode());
  writer->writeValue("GrObj::widthFactor", itsScaler.itsWidthFactor);
  writer->writeValue("GrObj::heightFactor", itsScaler.itsHeightFactor);

  writer->writeValue("GrObj::alignmentMode", itsAligner.itsMode);
  writer->writeValue("GrObj::centerX", itsAligner.itsCenter.x());
  writer->writeValue("GrObj::centerY", itsAligner.itsCenter.y());
}


///////////////////////////////////////////////////////////////////////
//
// GrObjImpl action definitions
//
///////////////////////////////////////////////////////////////////////

void GrObjImpl::draw(Gfx::Canvas& canvas) const
{
DOTRACE("GrObjImpl::draw");
  canvas.throwIfError("before GrObj::draw");

  bool objectDrawn = itsRenderer.update(this, canvas);

  Rect<double> bbox = itsObjNode->gnodeBoundingBox(canvas);

  if ( !objectDrawn )
    {
      Gfx::Canvas::StateSaver state(canvas);

      itsScaler.doScaling(canvas);
      itsAligner.doAlignment(canvas, bbox);

      itsRenderer.render(itsObjNode.get(), canvas);

      itsBB.draw(bbox, canvas);
    }

  canvas.throwIfError("during GrObj::draw");
}

void GrObjImpl::undraw(Gfx::Canvas& canvas) const
{
DOTRACE("GrObjImpl::undraw");
  canvas.throwIfError("before GrObj::undraw");

  switch (itsRenderer.getUnMode())
    {
    case Gmodes::DIRECT_RENDER:
    case Gmodes::SWAP_FORE_BACK:
      undrawDirectRender(canvas);
      break;
    case Gmodes::CLEAR_BOUNDING_BOX:
      undrawClearBoundingBox(canvas);
      break;
    default:
      break;
    }

  canvas.throwIfError("during GrObj::undraw");
}

void GrObjImpl::invalidateCaches()
{
DOTRACE("GrObjImpl::invalidateCaches");
  itsRenderer.invalidate();
}

void GrObjImpl::undrawDirectRender(Gfx::Canvas& canvas) const
{
DOTRACE("GrObjImpl::undrawDirectRender");

  Gfx::Canvas::StateSaver state(canvas);

  Rect<double> bbox = itsObjNode->gnodeBoundingBox(canvas);

  itsScaler.doScaling(canvas);
  itsAligner.doAlignment(canvas, bbox);

  itsRenderer.unrender(itsObjNode.get(), canvas);

  itsBB.draw(bbox, canvas);
}

void GrObjImpl::undrawClearBoundingBox(Gfx::Canvas& canvas) const
{
DOTRACE("GrObjImpl::undrawClearBoundingBox");

  Rect<double> world_pos =
    itsBB.withBorder(itsObjNode->gnodeBoundingBox(canvas), canvas);

  Rect<int> screen_pos = canvas.getScreenFromWorld(world_pos);

  // Add an extra one-pixel border around the rect
  screen_pos.widenByStep(itsBB.pixelBorder());
  screen_pos.heightenByStep(itsBB.pixelBorder());

  canvas.clearColorBuffer(screen_pos);
}

static const char vcid_grobjimpl_cc[] = "$Header$";
#endif // !GROBJIMPL_CC_DEFINED
