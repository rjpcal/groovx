///////////////////////////////////////////////////////////////////////
//
// grobj.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Dec  1 08:00:00 1998
// written: Tue Sep  4 12:53:51 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GROBJ_CC_DEFINED
#define GROBJ_CC_DEFINED

#include "grobj.h"

#include "grobjimpl.h"

#include "gfx/canvas.h"

#include "io/reader.h"
#include "io/writer.h"

#define DYNAMIC_TRACE_EXPR GrObj::tracer.status()
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

namespace
{
  const IO::VersionId GROBJ_SERIAL_VERSION_ID = 3;
}

Util::Tracer GrObj::tracer;

///////////////////////////////////////////////////////////////////////
//
// GrObj member definitions
//
///////////////////////////////////////////////////////////////////////


//////////////
// creators //
//////////////

// GrObj default constructor
GrObj::GrObj() :
  FieldContainer(&sigNodeChanged),
  itsImpl(GrObjImpl::make(this))
{
DOTRACE("GrObj::GrObj");

  setFieldMap(GrObj::classFields());

  // This is necessary because any representations that have been
  // cached during the GrObj constructor will become invalid upon
  // return to the derived class constructor.
  sigNodeChanged.emit();
}

// GrObj destructor
GrObj::~GrObj()
{
DOTRACE("GrObj::~GrObj");
  itsImpl->destroy();
}

IO::VersionId GrObj::serialVersionId() const
{
DOTRACE("GrObj::serialVersionId");
  return GROBJ_SERIAL_VERSION_ID;
}

void GrObj::readFrom(IO::Reader* reader)
{
DOTRACE("GrObj::readFrom");

  reader->ensureReadVersionId("GrObj", 3, "Try grsh0.8a7");

  readFieldsFrom(reader, classFields());
}

void GrObj::writeTo(IO::Writer* writer) const
{
DOTRACE("GrObj::writeTo");

  writer->ensureWriteVersionId("GrObj", GROBJ_SERIAL_VERSION_ID, 3,
                               "Try grsh0.8a7");

  writeFieldsTo(writer, classFields());
}

const FieldMap& GrObj::classFields()
{
#define GETSET(type, attr) std::make_pair(&type::get##attr, &type::set##attr)

  static const Field FIELD_ARRAY[] =
  {
    Field("category", std::make_pair(&GrObj::category, &GrObj::setCategory),
          0, 0, 20, 1, Field::NEW_GROUP),
    Field("renderMode", GETSET(GrObj, RenderMode), 1, 1, 4, 1),
    Field("cacheFilename", GETSET(GrObj, CacheFilename), 0, 0, 20, 1,
          Field::STRING),
    Field("bbVisibility", GETSET(GrObj, BBVisibility),
          false, false, true, true),
    Field("scalingMode", GETSET(GrObj, ScalingMode), 1, 1, 3, 1),
    Field("widthFactor", GETSET(GrObj, WidthFactor), 1.0, 0.1, 10.0, 0.1,
			 Field::PRIVATE),
    Field("heightFactor", GETSET(GrObj, HeightFactor), 1.0, 0.1, 10.0, 0.1,
			 Field::PRIVATE),
    Field("aspectRatio", GETSET(GrObj, AspectRatio), 1.0, 0.1, 10.0, 0.1,
          Field::TRANSIENT),
    Field("width", GETSET(GrObj, Width), 1.0, 0.1, 10.0, 0.1,
			 Field::TRANSIENT),
    Field("height", GETSET(GrObj, Height), 1.0, 0.1, 10.0, 0.1,
			 Field::TRANSIENT),
    Field("maxDimension", GETSET(GrObj, MaxDimension), 1.0, 0.1, 10.0, 0.1,
          Field::TRANSIENT),
    Field("alignmentMode", GETSET(GrObj, AlignmentMode), 1, 1, 7, 1),
    Field("centerX", GETSET(GrObj, CenterX), 0.0, -10.0, 10.0, 0.1),
    Field("centerY", GETSET(GrObj, CenterY), 0.0, -10.0, 10.0, 0.1)
  };
#undef GETSET

  static FieldMap GROBJ_FIELDS(FIELD_ARRAY);

  return GROBJ_FIELDS;
}

///////////////
// accessors //
///////////////

bool GrObj::getBBVisibility() const
{
DOTRACE("GrObj::getBBVisibility");
  return itsImpl->itsBB->isVisible();
}

void GrObj::getBoundingBox(Gfx::Rect<double>& bbox, Gfx::Canvas& canvas) const
{
DOTRACE("GrObj::getBoundingBox");

  bbox.unionize(itsImpl->itsTopNode->gnodeBoundingBox(canvas));
}

Gmodes::ScalingMode GrObj::getScalingMode() const
{
DOTRACE("GrObj::getScalingMode");
  return itsImpl->itsScaler->getMode();
}

double GrObj::getWidth() const
{
DOTRACE("GrObj::getWidth");
  return itsImpl->itsScaler->scaledWidth();
}

double GrObj::getHeight() const
{
DOTRACE("GrObj::getHeight");
  return itsImpl->itsScaler->scaledHeight();
}

double GrObj::getAspectRatio() const
{
DOTRACE("GrObj::getAspectRatio");
  return itsImpl->itsScaler->aspectRatio();
}

double GrObj::getMaxDimension() const
{
DOTRACE("GrObj::getMaxDimension");
  return itsImpl->itsScaler->scaledMaxDim();
}

Gmodes::AlignmentMode GrObj::getAlignmentMode() const
{
DOTRACE("GrObj::getAlignmentMode");
  return itsImpl->itsAligner->getMode();
}

double GrObj::getCenterX() const
{
DOTRACE("GrObj::getCenterX");
  return itsImpl->itsAligner->itsCenter.x();
}

double GrObj::getCenterY() const
{
DOTRACE("GrObj::getCenterY");
  return itsImpl->itsAligner->itsCenter.y();
}

int GrObj::getPixelBorder() const
{
DOTRACE("GrObj::getPixelBorder");
  return itsImpl->itsBB->pixelBorder();
}

int GrObj::category() const
{
DOTRACE("GrObj::category");
  return itsImpl->itsCategory;
}

Gmodes::RenderMode GrObj::getRenderMode() const
{
  return itsImpl->itsGLCache->getMode();
}

//////////////////
// manipulators //
//////////////////

void GrObj::setBitmapCacheDir(const char* dirname)
{
DOTRACE("GrObj::setBitmapCacheDir");
  BitmapCacheNode::BITMAP_CACHE_DIR = dirname;
}

void GrObj::setBBVisibility(bool visibility)
{
  itsImpl->itsBB->setVisible(visibility);
  this->sigNodeChanged.emit();
}

void GrObj::setScalingMode(Gmodes::ScalingMode val)
{
DOTRACE("GrObj::setScalingMode");

  itsImpl->itsScaler->setMode(val);
  this->sigNodeChanged.emit();
}

void GrObj::setWidth(double val)
{
DOTRACE("GrObj::setWidth");

  itsImpl->itsScaler->setWidth(val);
  this->sigNodeChanged.emit();
}

void GrObj::setHeight(double val)
{
DOTRACE("GrObj::setHeight");

  itsImpl->itsScaler->setHeight(val);
  this->sigNodeChanged.emit();
}

void GrObj::setAspectRatio(double val)
{
DOTRACE("GrObj::setAspectRatio");

  itsImpl->itsScaler->setAspectRatio(val);
  this->sigNodeChanged.emit();
}

void GrObj::setMaxDimension(double val)
{
DOTRACE("GrObj::setMaxDimension");

  itsImpl->itsScaler->setMaxDim(val);
  this->sigNodeChanged.emit();
}

void GrObj::setAlignmentMode(Gmodes::AlignmentMode val)
{
DOTRACE("GrObj::setAlignmentMode");

  itsImpl->itsAligner->setMode(val);
  this->sigNodeChanged.emit();
}

void GrObj::setCenterX(double val)
{
DOTRACE("GrObj::setCenterX");

  itsImpl->itsAligner->itsCenter.x() = val;
  this->sigNodeChanged.emit();
}

void GrObj::setCenterY(double val)
{
DOTRACE("GrObj::setCenterY");

  itsImpl->itsAligner->itsCenter.y() = val;
  this->sigNodeChanged.emit();
}

void GrObj::setPixelBorder(int pixels)
{
DOTRACE("GrObj::setPixelBorder");
  itsImpl->itsBB->setPixelBorder(pixels);
}

void GrObj::setCategory(int val)
{
DOTRACE("GrObj::setCategory");
  itsImpl->itsCategory = val;
}

void GrObj::setRenderMode(Gmodes::RenderMode mode)
{
DOTRACE("GrObj::setRenderMode");

  itsImpl->itsBitmapCache->setMode(mode);
  itsImpl->itsGLCache->setMode(mode);
  this->sigNodeChanged.emit();
}


/////////////
// actions //
/////////////

void GrObj::saveBitmapCache(Gfx::Canvas& canvas, const char* filename) const
{
DOTRACE("GrObj::saveBitmapCache");
  const_cast<GrObj*>(this)->setRenderMode(Gmodes::GL_BITMAP_CACHE);

  draw(canvas);

  itsImpl->itsBitmapCache->saveBitmapCache(canvas, filename);
}

void GrObj::update(Gfx::Canvas& canvas) const
{
DOTRACE("GrObj::update");
}

void GrObj::draw(Gfx::Canvas& canvas) const
{
DOTRACE("GrObj::draw");
  itsImpl->itsTopNode->gnodeDraw(canvas);
}

void GrObj::undraw(Gfx::Canvas& canvas) const
{
DOTRACE("GrObj::undraw");

  Gfx::Rect<double> world_box;
  getBoundingBox(world_box, canvas);

  Gfx::Rect<int> screen_box = canvas.screenFromWorld(world_box);

  canvas.clearColorBuffer(screen_box);
}

const fstring& GrObj::getCacheFilename() const
{
DOTRACE("GrObj::getCacheFilename");
  return itsImpl->itsBitmapCache->getCacheFilename();
}

void GrObj::setCacheFilename(const fstring& name)
{
DOTRACE("GrObj::setCacheFilename");
  itsImpl->itsBitmapCache->setCacheFilename(name);
}

double GrObj::getWidthFactor() const
{
DOTRACE("GrObj::getWidthFactor");
  return itsImpl->itsScaler->itsWidthFactor;
}

void GrObj::setWidthFactor(double val)
{
DOTRACE("GrObj::setWidthFactor");
  itsImpl->itsScaler->itsWidthFactor = val;
}

double GrObj::getHeightFactor() const
{
DOTRACE("GrObj::getHeightFactor");
  return itsImpl->itsScaler->itsHeightFactor;
}

void GrObj::setHeightFactor(double val)
{
DOTRACE("GrObj::setHeightFactor");
  itsImpl->itsScaler->itsHeightFactor = val;
}

static const char vcid_grobj_cc[] = "$Header$";
#endif // !GROBJ_CC_DEFINED
