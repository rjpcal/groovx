///////////////////////////////////////////////////////////////////////
//
// gxshapekit.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Dec  1 08:00:00 1998 (as grobj.cc)
// written: Wed Nov 20 15:55:14 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXSHAPEKIT_CC_DEFINED
#define GXSHAPEKIT_CC_DEFINED

#include "gfx/gxshapekit.h"

#include "gfx/canvas.h"
#include "gfx/gxaligner.h"
#include "gfx/gxbin.h"
#include "gfx/gxbounds.h"
#include "gfx/gxcache.h"
#include "gfx/gxscaler.h"

#include "gx/bbox.h"
#include "gx/box.h"

#include "io/reader.h"
#include "io/writer.h"

#include "util/volatileobject.h"

#define DYNAMIC_TRACE_EXPR GrObj::tracer.status()
#include "util/trace.h"
#include "util/debug.h"

//  ###################################################################
//  ===================================================================

/// GxBin wrapper for the GrObj core.

class GrObjNode : public GxBin
{
  borrowed_ptr<GrObj> itsObj;

public:
  GrObjNode(GrObj* obj) : GxBin(), itsObj(obj) {}

  virtual ~GrObjNode() {}

  virtual void readFrom(IO::Reader* /*reader*/) {};
  virtual void writeTo(IO::Writer* /*writer*/) const {};

  virtual void draw(Gfx::Canvas& canvas) const
  { itsObj->grRender(canvas); }

  virtual void getBoundingCube(Gfx::Bbox& bbox) const
  {
    itsObj->grGetBoundingBox(bbox);
    dbgDump(2, bbox.cube());
  }
};

//  ###################################################################
//  ===================================================================

/// Implementation class for GrObj.

class GrObjImpl : public Util::VolatileObject
{
private:
  GrObjImpl(const GrObjImpl&);
  GrObjImpl& operator=(const GrObjImpl&);

public:

  //
  // Data members
  //

  int itsCategory;

  Util::Ref<GrObjNode> itsNativeNode;
  Util::Ref<GxBounds> itsBB;
  Util::Ref<GxCache> itsCache;
  Util::Ref<GxAligner> itsAligner;
  Util::Ref<GxScaler> itsScaler;

  Util::Ref<GxNode> itsTopNode;

  //
  // Methods
  //

  static GrObjImpl* make(GrObj* obj) { return new GrObjImpl(obj); }

  GrObjImpl(GrObj* obj) :
    itsCategory(-1),
    itsNativeNode(new GrObjNode(obj), Util::PRIVATE),
    itsBB(new GxBounds(itsNativeNode), Util::PRIVATE),
    itsCache(new GxCache(itsBB), Util::PRIVATE),
    itsAligner(new GxAligner(itsCache), Util::PRIVATE),
    itsScaler(new GxScaler(itsAligner), Util::PRIVATE),
    itsTopNode(itsScaler)
  {
    // We connect to sigNodeChanged in order to update any caches
    // according to state changes.
    obj->sigNodeChanged.connect(this, &GrObjImpl::invalidateCaches);
  }

  void invalidateCaches()
  {
    itsCache->invalidate();
  }
};

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
#define GETSET(type, attr) make_mypair(&type::get##attr, &type::set##attr)

  static const Field FIELD_ARRAY[] =
  {
    Field("category", make_mypair(&GrObj::category, &GrObj::setCategory),
          0, 0, 20, 1, Field::NEW_GROUP),
    Field("renderMode", GETSET(GrObj, RenderMode), 1, 1, 4, 1),
    Field("bbVisibility", GETSET(GrObj, BBVisibility),
          false, false, true, true, Field::BOOLEAN),
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

void GrObj::getBoundingCube(Gfx::Bbox& bbox) const
{
DOTRACE("GrObj::getBoundingCube");

  itsImpl->itsTopNode->getBoundingCube(bbox);
}

int GrObj::getScalingMode() const
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

int GrObj::getAlignmentMode() const
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

int GrObj::getRenderMode() const
{
  return itsImpl->itsCache->getMode();
}

//////////////////
// manipulators //
//////////////////

void GrObj::setBBVisibility(bool visibility)
{
  itsImpl->itsBB->setVisible(visibility);
  this->sigNodeChanged.emit();
}

void GrObj::setScalingMode(int val)
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

void GrObj::setAlignmentMode(int val)
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

void GrObj::setRenderMode(int mode)
{
DOTRACE("GrObj::setRenderMode");

  itsImpl->itsCache->setMode(mode);
  this->sigNodeChanged.emit();
}


/////////////
// actions //
/////////////

void GrObj::draw(Gfx::Canvas& canvas) const
{
DOTRACE("GrObj::draw");
  itsImpl->itsTopNode->draw(canvas);
}

double GrObj::getWidthFactor() const
{
DOTRACE("GrObj::getWidthFactor");
  return itsImpl->itsScaler->widthFactor();
}

void GrObj::setWidthFactor(double val)
{
DOTRACE("GrObj::setWidthFactor");
  itsImpl->itsScaler->setWidthFactor(val);
}

double GrObj::getHeightFactor() const
{
DOTRACE("GrObj::getHeightFactor");
  return itsImpl->itsScaler->heightFactor();
}

void GrObj::setHeightFactor(double val)
{
DOTRACE("GrObj::setHeightFactor");
  itsImpl->itsScaler->setHeightFactor(val);
}

static const char vcid_gxshapekit_cc[] = "$Header$";
#endif // !GXSHAPEKIT_CC_DEFINED