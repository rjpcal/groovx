///////////////////////////////////////////////////////////////////////
//
// grobjimpl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Mar 23 16:27:57 2000
// written: Thu Jul 19 11:46:42 2001
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

#include <GL/gl.h>
#include <GL/glu.h>

#define DYNAMIC_TRACE_EXPR GrObj::tracer.status()
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// File scope stuff
//
///////////////////////////////////////////////////////////////////////

const IO::VersionId GrObjImpl::GROBJ_SERIAL_VERSION_ID;


///////////////////////////////////////////////////////////////////////
//
// GrObjImpl creator definitions
//
///////////////////////////////////////////////////////////////////////

GrObjImpl::GrObjImpl(GrObj* obj) :
  self(obj),
  itsCategory(-1),
  itsBB(this),
  itsScaler(),
  itsAligner(),
  itsRenderer(),
  itsUnRenderer()
{};

GrObjImpl::~GrObjImpl() {
DOTRACE("GrObjImpl::~GrObjImpl");
}

void GrObjImpl::readFrom(IO::Reader* reader) {
DOTRACE("GrObjImpl::readFrom");

  reader->ensureReadVersionId("GrObj", 1, "Try grsh0.8a4");

  reader->readValue("GrObj::category", itsCategory);

  int temp;
  reader->readValue("GrObj::renderMode", temp);
  itsRenderer.setMode(temp);

  dynamic_string filename;
  reader->readValue("GrObj::cacheFilename", filename);
  itsRenderer.setCacheFilename(filename);

  reader->readValue("GrObj::unRenderMode", itsUnRenderer.itsMode);

  {
    bool val;
    reader->readValue("GrObj::bbVisibility", val);
    itsBB.setVisible(val);
  }

  reader->readValue("GrObj::scalingMode", temp);
  itsScaler.setMode(temp);

  reader->readValue("GrObj::widthFactor", itsScaler.itsWidthFactor);
  reader->readValue("GrObj::heightFactor", itsScaler.itsHeightFactor);

  reader->readValue("GrObj::alignmentMode", itsAligner.itsMode);
  reader->readValue("GrObj::centerX", itsAligner.itsCenter.x());
  reader->readValue("GrObj::centerY", itsAligner.itsCenter.y());

  invalidateCaches();
}

void GrObjImpl::writeTo(IO::Writer* writer) const {
DOTRACE("GrObjImpl::writeTo");

  writer->ensureWriteVersionId("GrObj", GROBJ_SERIAL_VERSION_ID, 1,
                               "Try grsh0.8a4");

  writer->writeValue("GrObj::category", itsCategory);

  writer->writeValue("GrObj::renderMode", itsRenderer.getMode());

  writer->writeValue("GrObj::cacheFilename", itsRenderer.getCacheFilename());

  writer->writeValue("GrObj::unRenderMode", itsUnRenderer.itsMode);

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

void GrObjImpl::draw(GWT::Canvas& canvas) const {
DOTRACE("GrObjImpl::draw");
  canvas.throwIfError("before GrObj::draw");

  bool objectDrawn = itsRenderer.update(this, canvas);

  Rect<double> bbox = grGetBoundingBox();

  if ( !objectDrawn )
    {
      GWT::Canvas::StateSaver state(canvas);

      itsScaler.doScaling(canvas);
      itsAligner.doAlignment(canvas, bbox);

      itsRenderer.render(this, canvas);

      itsBB.draw(bbox, canvas);
    }

  canvas.throwIfError("during GrObj::draw");
}

void GrObjImpl::undraw(GWT::Canvas& canvas) const {
DOTRACE("GrObjImpl::undraw");
  canvas.throwIfError("before GrObj::undraw");

  switch (itsUnRenderer.itsMode)
    {
    case GrObj::DIRECT_RENDER:     undrawDirectRender(canvas);     break;
    case GrObj::SWAP_FORE_BACK:    undrawSwapForeBack(canvas);     break;
    case GrObj::CLEAR_BOUNDING_BOX:undrawClearBoundingBox(canvas); break;
    default:                       /* nothing */                   break;
    }

  canvas.throwIfError("during GrObj::undraw");
}

void GrObjImpl::invalidateCaches() {
DOTRACE("GrObjImpl::invalidateCaches");
  itsRenderer.invalidate();
}

void GrObjImpl::undrawDirectRender(GWT::Canvas& canvas) const {
DOTRACE("GrObjImpl::undrawDirectRender");

  GWT::Canvas::StateSaver state(canvas);

  Rect<double> bbox = grGetBoundingBox();

  itsScaler.doScaling(canvas);
  itsAligner.doAlignment(canvas, bbox);

  if ( itsRenderer.getMode() == GrObj::GLCOMPILE )
    {
      itsRenderer.callList();
    }
  else
    {
      self->grRender(canvas, GrObj::UNDRAW);
    }

  itsBB.draw(bbox, canvas);
}

void GrObjImpl::undrawSwapForeBack(GWT::Canvas& canvas) const {
DOTRACE("GrObjImpl::undrawSwapForeBack");
  glPushAttrib(GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT);
  {
    canvas.swapForeBack();

    undrawDirectRender(canvas);
  }
  glPopAttrib();
}

void GrObjImpl::undrawClearBoundingBox(GWT::Canvas& canvas) const {
DOTRACE("GrObjImpl::undrawClearBoundingBox");

  Rect<double> world_pos = itsBB.withBorder(grGetBoundingBox(), canvas);

  Rect<int> screen_pos = canvas.getScreenFromWorld(world_pos);

  // Add an extra one-pixel border around the rect
  screen_pos.widenByStep(itsBB.pixelBorder());
  screen_pos.heightenByStep(itsBB.pixelBorder());

  canvas.clearColorBuffer(screen_pos);
}

static const char vcid_grobjimpl_cc[] = "$Header$";
#endif // !GROBJIMPL_CC_DEFINED
