///////////////////////////////////////////////////////////////////////
//
// grobjimpl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Mar 23 16:27:57 2000
// written: Fri Aug 10 18:55:17 2001
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
  itsCategory(-1),
  itsNativeNode(new GrObjNode(obj)),
  itsBB(new GrObjBBox(itsNativeNode)),
  itsGLCache(new GLCacheNode(itsBB)),
  itsAligner(new GrObjAligner(itsGLCache)),
  itsScaler(new GrObjScaler(itsAligner)),
  itsBitmapCache(new BitmapCacheNode(itsScaler))
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
    itsGLCache->setMode(temp);
    itsBitmapCache->setMode(temp);
  }

  {
    fstring filename;
    reader->readValue("GrObj::cacheFilename", filename);
    itsBitmapCache->setCacheFilename(filename);
  }

  {
    int temp;
    reader->readValue("GrObj::unRenderMode", temp);
    itsGLCache->setUnMode(temp);
  }

  {
    bool val;
    reader->readValue("GrObj::bbVisibility", val);
    itsBB->setVisible(val);
  }

  {
    int temp;
    reader->readValue("GrObj::scalingMode", temp);
    itsScaler->setMode(temp);
  }

  reader->readValue("GrObj::widthFactor", itsScaler->itsWidthFactor);
  reader->readValue("GrObj::heightFactor", itsScaler->itsHeightFactor);

  reader->readValue("GrObj::alignmentMode", itsAligner->itsMode);
  reader->readValue("GrObj::centerX", itsAligner->itsCenter.x());
  reader->readValue("GrObj::centerY", itsAligner->itsCenter.y());

  invalidateCaches();
}

void GrObjImpl::writeTo(IO::Writer* writer) const
{
DOTRACE("GrObjImpl::writeTo");

  writer->ensureWriteVersionId("GrObj", GROBJ_SERIAL_VERSION_ID, 1,
                               "Try grsh0.8a4");

  writer->writeValue("GrObj::category", itsCategory);

  writer->writeValue("GrObj::renderMode", itsGLCache->getMode());

  writer->writeValue("GrObj::cacheFilename", itsBitmapCache->getCacheFilename());

  writer->writeValue("GrObj::unRenderMode", itsGLCache->getUnMode());

  writer->writeValue("GrObj::bbVisibility", itsBB->isVisible());

  writer->writeValue("GrObj::scalingMode", itsScaler->getMode());
  writer->writeValue("GrObj::widthFactor", itsScaler->itsWidthFactor);
  writer->writeValue("GrObj::heightFactor", itsScaler->itsHeightFactor);

  writer->writeValue("GrObj::alignmentMode", itsAligner->itsMode);
  writer->writeValue("GrObj::centerX", itsAligner->itsCenter.x());
  writer->writeValue("GrObj::centerY", itsAligner->itsCenter.y());
}


///////////////////////////////////////////////////////////////////////
//
// GrObjImpl action definitions
//
///////////////////////////////////////////////////////////////////////

void GrObjImpl::draw(Gfx::Canvas& canvas) const
{
DOTRACE("GrObjImpl::draw");
  itsBitmapCache->gnodeDraw(canvas);
}

void GrObjImpl::undraw(Gfx::Canvas& canvas) const
{
DOTRACE("GrObjImpl::undraw");
  itsBitmapCache->gnodeUndraw(canvas);
}

void GrObjImpl::invalidateCaches()
{
DOTRACE("GrObjImpl::invalidateCaches");
  itsGLCache->invalidate();
  itsBitmapCache->invalidate();
}

static const char vcid_grobjimpl_cc[] = "$Header$";
#endif // !GROBJIMPL_CC_DEFINED
