///////////////////////////////////////////////////////////////////////
//
// grobjimpl.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Mar 23 16:27:54 2000
// written: Sat Aug 11 08:38:37 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GROBJIMPL_H_DEFINED
#define GROBJIMPL_H_DEFINED

#include "application.h"

#include "gnode.h"

#include "grobj.h"
#include "grobjaligner.h"
#include "grobjbbox.h"
#include "grobjscaler.h"

#include "bitmapcachenode.h"
#include "glcachenode.h"

#include "io/reader.h"
#include "io/writer.h"

class GrObjNode : public Gnode {
  borrowed_ptr<GrObj> itsObj;

public:
  GrObjNode(GrObj* obj) : Gnode(), itsObj(obj) {}

  virtual ~GrObjNode() {}

  virtual void gnodeDraw(Gfx::Canvas& canvas) const
  { itsObj->grRender(canvas, GrObj::DRAW); }

  virtual void gnodeUndraw(Gfx::Canvas& canvas) const
  { itsObj->grRender(canvas, GrObj::UNDRAW); }

  virtual Rect<double> gnodeBoundingBox(Gfx::Canvas& /*canvas*/) const
  { return itsObj->grGetBoundingBox(); }
};

///////////////////////////////////////////////////////////////////////
//
// GrObjImpl class
//
///////////////////////////////////////////////////////////////////////

namespace
{
  const IO::VersionId GROBJ_SERIAL_VERSION_ID = 1;
}

class GrObjImpl {
private:
  GrObjImpl(const GrObjImpl&);
  GrObjImpl& operator=(const GrObjImpl&);

public:

  GrObjImpl(GrObj* obj) :
    itsCategory(-1),
    itsNativeNode(new GrObjNode(obj)),
    itsBB(new GrObjBBox(itsNativeNode)),
    itsGLCache(new GLCacheNode(itsBB)),
    itsAligner(new GrObjAligner(itsGLCache)),
    itsScaler(new GrObjScaler(itsAligner)),
    itsBitmapCache(new BitmapCacheNode(itsScaler))
  {}

  IO::VersionId serialVersionId() const
  {
    return GROBJ_SERIAL_VERSION_ID;
  }

  void readFrom(IO::Reader* reader)
  {
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

  void writeTo(IO::Writer* writer) const
  {
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


  void draw(Gfx::Canvas& canvas) const
  {
    itsBitmapCache->gnodeDraw(canvas);
  }

  void undraw(Gfx::Canvas& canvas) const
  {
    itsBitmapCache->gnodeUndraw(canvas);
  }

  void invalidateCaches()
  {
    itsGLCache->invalidate();
    itsBitmapCache->invalidate();
  }

  //
  // Data members
  //

  int itsCategory;

  shared_ptr<GrObjNode> itsNativeNode;
  shared_ptr<GrObjBBox> itsBB;
  shared_ptr<GLCacheNode> itsGLCache;
  shared_ptr<GrObjAligner> itsAligner;
  shared_ptr<GrObjScaler> itsScaler;
  shared_ptr<BitmapCacheNode> itsBitmapCache;
};

static const char vcid_grobjimpl_h[] = "$Header$";
#endif // !GROBJIMPL_H_DEFINED
