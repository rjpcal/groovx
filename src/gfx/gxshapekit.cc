///////////////////////////////////////////////////////////////////////
//
// gxshapekit.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Dec  1 08:00:00 1998 (as grobj.cc)
// written: Wed Nov 20 16:10:45 2002
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

#define DYNAMIC_TRACE_EXPR GxShapeKit::tracer.status()
#include "util/trace.h"
#include "util/debug.h"

//  ###################################################################
//  ===================================================================

/// GxBin wrapper for the GxShapeKit core.

class GxShapeKitNode : public GxBin
{
  borrowed_ptr<GxShapeKit> itsObj;

public:
  GxShapeKitNode(GxShapeKit* obj) : GxBin(), itsObj(obj) {}

  virtual ~GxShapeKitNode() {}

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

/// Implementation class for GxShapeKit.

class GxShapeKitImpl : public Util::VolatileObject
{
private:
  GxShapeKitImpl(const GxShapeKitImpl&);
  GxShapeKitImpl& operator=(const GxShapeKitImpl&);

public:

  //
  // Data members
  //

  int itsCategory;

  Util::Ref<GxShapeKitNode> itsNativeNode;
  Util::Ref<GxBounds> itsBB;
  Util::Ref<GxCache> itsCache;
  Util::Ref<GxAligner> itsAligner;
  Util::Ref<GxScaler> itsScaler;

  Util::Ref<GxNode> itsTopNode;

  //
  // Methods
  //

  static GxShapeKitImpl* make(GxShapeKit* obj) { return new GxShapeKitImpl(obj); }

  GxShapeKitImpl(GxShapeKit* obj) :
    itsCategory(-1),
    itsNativeNode(new GxShapeKitNode(obj), Util::PRIVATE),
    itsBB(new GxBounds(itsNativeNode), Util::PRIVATE),
    itsCache(new GxCache(itsBB), Util::PRIVATE),
    itsAligner(new GxAligner(itsCache), Util::PRIVATE),
    itsScaler(new GxScaler(itsAligner), Util::PRIVATE),
    itsTopNode(itsScaler)
  {
    // We connect to sigNodeChanged in order to update any caches
    // according to state changes.
    obj->sigNodeChanged.connect(this, &GxShapeKitImpl::invalidateCaches);
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

Util::Tracer GxShapeKit::tracer;

///////////////////////////////////////////////////////////////////////
//
// GxShapeKit member definitions
//
///////////////////////////////////////////////////////////////////////


//////////////
// creators //
//////////////

// GxShapeKit default constructor
GxShapeKit::GxShapeKit() :
  FieldContainer(&sigNodeChanged),
  itsImpl(GxShapeKitImpl::make(this))
{
DOTRACE("GxShapeKit::GxShapeKit");

  setFieldMap(GxShapeKit::classFields());

  // This is necessary because any representations that have been
  // cached during the GxShapeKit constructor will become invalid upon
  // return to the derived class constructor.
  sigNodeChanged.emit();
}

// GxShapeKit destructor
GxShapeKit::~GxShapeKit()
{
DOTRACE("GxShapeKit::~GxShapeKit");
  itsImpl->destroy();
}

IO::VersionId GxShapeKit::serialVersionId() const
{
DOTRACE("GxShapeKit::serialVersionId");
  return GROBJ_SERIAL_VERSION_ID;
}

void GxShapeKit::readFrom(IO::Reader* reader)
{
DOTRACE("GxShapeKit::readFrom");

  reader->ensureReadVersionId("GxShapeKit", 3, "Try grsh0.8a7");

  readFieldsFrom(reader, classFields());
}

void GxShapeKit::writeTo(IO::Writer* writer) const
{
DOTRACE("GxShapeKit::writeTo");

  writer->ensureWriteVersionId("GxShapeKit", GROBJ_SERIAL_VERSION_ID, 3,
                               "Try grsh0.8a7");

  writeFieldsTo(writer, classFields());
}

const FieldMap& GxShapeKit::classFields()
{
#define GETSET(type, attr) make_mypair(&type::get##attr, &type::set##attr)

  static const Field FIELD_ARRAY[] =
  {
    Field("category", make_mypair(&GxShapeKit::category, &GxShapeKit::setCategory),
          0, 0, 20, 1, Field::NEW_GROUP),
    Field("renderMode", GETSET(GxShapeKit, RenderMode), 1, 1, 4, 1),
    Field("bbVisibility", GETSET(GxShapeKit, BBVisibility),
          false, false, true, true, Field::BOOLEAN),
    Field("scalingMode", GETSET(GxShapeKit, ScalingMode), 1, 1, 3, 1),
    Field("widthFactor", GETSET(GxShapeKit, WidthFactor), 1.0, 0.1, 10.0, 0.1,
          Field::PRIVATE),
    Field("heightFactor", GETSET(GxShapeKit, HeightFactor), 1.0, 0.1, 10.0, 0.1,
          Field::PRIVATE),
    Field("aspectRatio", GETSET(GxShapeKit, AspectRatio), 1.0, 0.1, 10.0, 0.1,
          Field::TRANSIENT),
    Field("width", GETSET(GxShapeKit, Width), 1.0, 0.1, 10.0, 0.1,
          Field::TRANSIENT),
    Field("height", GETSET(GxShapeKit, Height), 1.0, 0.1, 10.0, 0.1,
          Field::TRANSIENT),
    Field("maxDimension", GETSET(GxShapeKit, MaxDimension), 1.0, 0.1, 10.0, 0.1,
          Field::TRANSIENT),
    Field("alignmentMode", GETSET(GxShapeKit, AlignmentMode), 1, 1, 7, 1),
    Field("centerX", GETSET(GxShapeKit, CenterX), 0.0, -10.0, 10.0, 0.1),
    Field("centerY", GETSET(GxShapeKit, CenterY), 0.0, -10.0, 10.0, 0.1)
  };
#undef GETSET

  static FieldMap GROBJ_FIELDS(FIELD_ARRAY);

  return GROBJ_FIELDS;
}

///////////////
// accessors //
///////////////

bool GxShapeKit::getBBVisibility() const
{
DOTRACE("GxShapeKit::getBBVisibility");
  return itsImpl->itsBB->isVisible();
}

void GxShapeKit::getBoundingCube(Gfx::Bbox& bbox) const
{
DOTRACE("GxShapeKit::getBoundingCube");

  itsImpl->itsTopNode->getBoundingCube(bbox);
}

int GxShapeKit::getScalingMode() const
{
DOTRACE("GxShapeKit::getScalingMode");
  return itsImpl->itsScaler->getMode();
}

double GxShapeKit::getWidth() const
{
DOTRACE("GxShapeKit::getWidth");
  return itsImpl->itsScaler->scaledWidth();
}

double GxShapeKit::getHeight() const
{
DOTRACE("GxShapeKit::getHeight");
  return itsImpl->itsScaler->scaledHeight();
}

double GxShapeKit::getAspectRatio() const
{
DOTRACE("GxShapeKit::getAspectRatio");
  return itsImpl->itsScaler->aspectRatio();
}

double GxShapeKit::getMaxDimension() const
{
DOTRACE("GxShapeKit::getMaxDimension");
  return itsImpl->itsScaler->scaledMaxDim();
}

int GxShapeKit::getAlignmentMode() const
{
DOTRACE("GxShapeKit::getAlignmentMode");
  return itsImpl->itsAligner->getMode();
}

double GxShapeKit::getCenterX() const
{
DOTRACE("GxShapeKit::getCenterX");
  return itsImpl->itsAligner->itsCenter.x();
}

double GxShapeKit::getCenterY() const
{
DOTRACE("GxShapeKit::getCenterY");
  return itsImpl->itsAligner->itsCenter.y();
}

int GxShapeKit::getPixelBorder() const
{
DOTRACE("GxShapeKit::getPixelBorder");
  return itsImpl->itsBB->pixelBorder();
}

int GxShapeKit::category() const
{
DOTRACE("GxShapeKit::category");
  return itsImpl->itsCategory;
}

int GxShapeKit::getRenderMode() const
{
  return itsImpl->itsCache->getMode();
}

//////////////////
// manipulators //
//////////////////

void GxShapeKit::setBBVisibility(bool visibility)
{
  itsImpl->itsBB->setVisible(visibility);
  this->sigNodeChanged.emit();
}

void GxShapeKit::setScalingMode(int val)
{
DOTRACE("GxShapeKit::setScalingMode");

  itsImpl->itsScaler->setMode(val);
  this->sigNodeChanged.emit();
}

void GxShapeKit::setWidth(double val)
{
DOTRACE("GxShapeKit::setWidth");

  itsImpl->itsScaler->setWidth(val);
  this->sigNodeChanged.emit();
}

void GxShapeKit::setHeight(double val)
{
DOTRACE("GxShapeKit::setHeight");

  itsImpl->itsScaler->setHeight(val);
  this->sigNodeChanged.emit();
}

void GxShapeKit::setAspectRatio(double val)
{
DOTRACE("GxShapeKit::setAspectRatio");

  itsImpl->itsScaler->setAspectRatio(val);
  this->sigNodeChanged.emit();
}

void GxShapeKit::setMaxDimension(double val)
{
DOTRACE("GxShapeKit::setMaxDimension");

  itsImpl->itsScaler->setMaxDim(val);
  this->sigNodeChanged.emit();
}

void GxShapeKit::setAlignmentMode(int val)
{
DOTRACE("GxShapeKit::setAlignmentMode");

  itsImpl->itsAligner->setMode(val);
  this->sigNodeChanged.emit();
}

void GxShapeKit::setCenterX(double val)
{
DOTRACE("GxShapeKit::setCenterX");

  itsImpl->itsAligner->itsCenter.x() = val;
  this->sigNodeChanged.emit();
}

void GxShapeKit::setCenterY(double val)
{
DOTRACE("GxShapeKit::setCenterY");

  itsImpl->itsAligner->itsCenter.y() = val;
  this->sigNodeChanged.emit();
}

void GxShapeKit::setPixelBorder(int pixels)
{
DOTRACE("GxShapeKit::setPixelBorder");
  itsImpl->itsBB->setPixelBorder(pixels);
}

void GxShapeKit::setCategory(int val)
{
DOTRACE("GxShapeKit::setCategory");
  itsImpl->itsCategory = val;
}

void GxShapeKit::setRenderMode(int mode)
{
DOTRACE("GxShapeKit::setRenderMode");

  itsImpl->itsCache->setMode(mode);
  this->sigNodeChanged.emit();
}


/////////////
// actions //
/////////////

void GxShapeKit::draw(Gfx::Canvas& canvas) const
{
DOTRACE("GxShapeKit::draw");
  itsImpl->itsTopNode->draw(canvas);
}

double GxShapeKit::getWidthFactor() const
{
DOTRACE("GxShapeKit::getWidthFactor");
  return itsImpl->itsScaler->widthFactor();
}

void GxShapeKit::setWidthFactor(double val)
{
DOTRACE("GxShapeKit::setWidthFactor");
  itsImpl->itsScaler->setWidthFactor(val);
}

double GxShapeKit::getHeightFactor() const
{
DOTRACE("GxShapeKit::getHeightFactor");
  return itsImpl->itsScaler->heightFactor();
}

void GxShapeKit::setHeightFactor(double val)
{
DOTRACE("GxShapeKit::setHeightFactor");
  itsImpl->itsScaler->setHeightFactor(val);
}

static const char vcid_gxshapekit_cc[] = "$Header$";
#endif // !GXSHAPEKIT_CC_DEFINED
