/** @file gfx/gxshapekit.cc full-featured base class for high-level
    graphic objects */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1998-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Tue Dec  1 08:00:00 1998 (as grobj.cc)
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#include "gfx/gxshapekit.h"

#include "geom/box.h"

#include "gfx/bbox.h"
#include "gfx/canvas.h"
#include "gfx/gxaligner.h"
#include "gfx/gxbin.h"
#include "gfx/gxbounds.h"
#include "gfx/gxcache.h"
#include "gfx/gxscaler.h"

#include "io/reader.h"
#include "io/writer.h"

#include "nub/volatileobject.h"

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

//  #######################################################
//  =======================================================

/// GxBin wrapper for the GxShapeKit core.

class GxShapeKitNode : public GxBin
{
  GxShapeKit* itsObj;

public:
  GxShapeKitNode(GxShapeKit* obj) : GxBin(), itsObj(obj) {}

  virtual ~GxShapeKitNode() noexcept {}

  virtual void read_from(io::reader& /*reader*/) {}
  virtual void write_to(io::writer& /*writer*/) const {}

  virtual void draw(Gfx::Canvas& canvas) const
  { itsObj->grRender(canvas); }

  virtual void getBoundingCube(Gfx::Bbox& bbox) const
  {
    itsObj->grGetBoundingBox(bbox);
    dbg_dump(2, bbox.cube());
  }
};

//  #######################################################
//  =======================================================

/// Implementation class for GxShapeKit.

class GxShapeKitImpl : public nub::volatile_object
{
private:
  GxShapeKitImpl(const GxShapeKitImpl&);
  GxShapeKitImpl& operator=(const GxShapeKitImpl&);

public:

  //
  // Data members
  //

  int category;

  nub::ref<GxShapeKitNode> nativeNode;
  nub::ref<GxBounds> boundsOutline;
  nub::ref<GxCache> cache;
  nub::ref<GxAligner> aligner;
  nub::ref<GxScaler> scaler;

  nub::ref<GxNode> topNode;

  //
  // Methods
  //

  static GxShapeKitImpl* make(GxShapeKit* obj)
  {
    GVX_TRACE("GxShapeKitImpl::make");
    return new GxShapeKitImpl(obj);
  }

  GxShapeKitImpl(GxShapeKit* obj) :
    category(-1),
    nativeNode(new GxShapeKitNode(obj), nub::PRIVATE),
    boundsOutline(new GxBounds(nativeNode), nub::PRIVATE),
    cache(new GxCache(boundsOutline), nub::PRIVATE),
    aligner(new GxAligner(cache), nub::PRIVATE),
    scaler(new GxScaler(aligner), nub::PRIVATE),
    topNode(scaler)
  {
    GVX_TRACE("GxShapeKitImpl::GxShapeKitImpl");
    // We connect to sigNodeChanged in order to update any caches
    // according to state changes.
    obj->sigNodeChanged.connect(this, &GxShapeKitImpl::invalidateCaches);
  }

  virtual ~GxShapeKitImpl() noexcept {}

  void invalidateCaches()
  {
    cache->invalidate();
  }
};

namespace
{
  const io::version_id GXSHAPEKIT_SVID = 4;
}

rutz::tracer GxShapeKit::tracer;

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
  rep(GxShapeKitImpl::make(this))
{
GVX_TRACE("GxShapeKit::GxShapeKit");

  setFieldMap(GxShapeKit::classFields());

  // This is necessary because any representations that have been
  // cached during the GxShapeKit constructor will become invalid upon
  // return to the derived class constructor.
  sigNodeChanged.emit();
}

// GxShapeKit destructor
GxShapeKit::~GxShapeKit() noexcept
{
GVX_TRACE("GxShapeKit::~GxShapeKit");
  rep->destroy();
}

io::version_id GxShapeKit::class_version_id() const
{
GVX_TRACE("GxShapeKit::class_version_id");
  return GXSHAPEKIT_SVID;
}

void GxShapeKit::read_from(io::reader& reader)
{
GVX_TRACE("GxShapeKit::read_from");

  const io::version_id svid =
    reader.ensure_version_id("GxShapeKit", 3,
                             "Try groovx0.8a7", SRC_POS);

  readFieldsFrom(reader, classFields());

  if (svid >= 4)
    {
      reader.read_owned_object("bounds", rep->boundsOutline);
      reader.read_owned_object("cache", rep->cache);
      reader.read_owned_object("aligner", rep->aligner);
      reader.read_owned_object("scaler", rep->scaler);
    }
}

void GxShapeKit::write_to(io::writer& writer) const
{
GVX_TRACE("GxShapeKit::write_to");

  writer.ensure_output_version_id("GxShapeKit",
                              GXSHAPEKIT_SVID, 4,
                              "Try groovx0.8a7", SRC_POS);

  writeFieldsTo(writer, classFields(), GXSHAPEKIT_SVID);

  writer.write_owned_object("bounds", rep->boundsOutline);
  writer.write_owned_object("cache", rep->cache);
  writer.write_owned_object("aligner", rep->aligner);
  writer.write_owned_object("scaler", rep->scaler);
}

const FieldMap& GxShapeKit::classFields()
{
#define GETSET(attr) &GxShapeKit::get##attr, &GxShapeKit::set##attr

  static const Field FIELD_ARRAY[] =
  {
    Field("category", &GxShapeKit::category, &GxShapeKit::setCategory,
          0, 0, 20, 1, Field::NEW_GROUP),
    Field("renderMode", GETSET(RenderMode), 1, 1, 4, 1).versions(0,3),
    Field("bbVisibility", GETSET(BBVisibility),
          false, false, true, true, Field::BOOLEAN).versions(0,3),
    Field("scalingMode", GETSET(ScalingMode), 1, 1, 3, 1).versions(0,3),
    Field("widthFactor", GETSET(WidthFactor), 1.0, 0.1, 10.0, 0.1,
          Field::PRIVATE).versions(0,3),
    Field("heightFactor", GETSET(HeightFactor), 1.0, 0.1, 10.0, 0.1,
          Field::PRIVATE).versions(0,3),
    Field("aspectRatio", GETSET(AspectRatio), 1.0, 0.1, 10.0, 0.1,
          Field::TRANSIENT),
    Field("width", GETSET(Width), 1.0, 0.1, 10.0, 0.1,
          Field::TRANSIENT),
    Field("height", GETSET(Height), 1.0, 0.1, 10.0, 0.1,
          Field::TRANSIENT),
    Field("maxDimension", GETSET(MaxDimension), 1.0, 0.1, 10.0, 0.1,
          Field::TRANSIENT),
    Field("alignmentMode", GETSET(AlignmentMode), 1, 1, 7, 1).versions(0,3),
    Field("centerX", GETSET(CenterX), 0.0, -10.0, 10.0, 0.1).versions(0,3),
    Field("centerY", GETSET(CenterY), 0.0, -10.0, 10.0, 0.1).versions(0,3)
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
GVX_TRACE("GxShapeKit::getBBVisibility");
  return rep->boundsOutline->isVisible();
}

void GxShapeKit::getBoundingCube(Gfx::Bbox& bbox) const
{
GVX_TRACE("GxShapeKit::getBoundingCube");

  rep->topNode->getBoundingCube(bbox);
}

int GxShapeKit::getScalingMode() const
{
GVX_TRACE("GxShapeKit::getScalingMode");
  return rep->scaler->getMode();
}

double GxShapeKit::getWidth() const
{
GVX_TRACE("GxShapeKit::getWidth");
  return rep->scaler->scaledWidth();
}

double GxShapeKit::getHeight() const
{
GVX_TRACE("GxShapeKit::getHeight");
  return rep->scaler->scaledHeight();
}

double GxShapeKit::getAspectRatio() const
{
GVX_TRACE("GxShapeKit::getAspectRatio");
  return rep->scaler->aspectRatio();
}

double GxShapeKit::getMaxDimension() const
{
GVX_TRACE("GxShapeKit::getMaxDimension");
  return rep->scaler->scaledMaxDim();
}

int GxShapeKit::getAlignmentMode() const
{
GVX_TRACE("GxShapeKit::getAlignmentMode");
  return rep->aligner->getMode();
}

double GxShapeKit::getCenterX() const
{
GVX_TRACE("GxShapeKit::getCenterX");
  return rep->aligner->itsCenter.x();
}

double GxShapeKit::getCenterY() const
{
GVX_TRACE("GxShapeKit::getCenterY");
  return rep->aligner->itsCenter.y();
}

int GxShapeKit::getPercentBorder() const
{
GVX_TRACE("GxShapeKit::getPercentBorder");
  return rep->boundsOutline->percentBorder();
}

int GxShapeKit::category() const
{
GVX_TRACE("GxShapeKit::category");
  return rep->category;
}

int GxShapeKit::getRenderMode() const
{
  return rep->cache->getMode();
}

//////////////////
// manipulators //
//////////////////

void GxShapeKit::setBBVisibility(bool visibility)
{
  rep->boundsOutline->setVisible(visibility);
  this->sigNodeChanged.emit();
}

void GxShapeKit::setScalingMode(int val)
{
GVX_TRACE("GxShapeKit::setScalingMode");

  rep->scaler->setMode(val);
  this->sigNodeChanged.emit();
}

void GxShapeKit::setWidth(double val)
{
GVX_TRACE("GxShapeKit::setWidth");

  rep->scaler->setWidth(val);
  this->sigNodeChanged.emit();
}

void GxShapeKit::setHeight(double val)
{
GVX_TRACE("GxShapeKit::setHeight");

  rep->scaler->setHeight(val);
  this->sigNodeChanged.emit();
}

void GxShapeKit::setAspectRatio(double val)
{
GVX_TRACE("GxShapeKit::setAspectRatio");

  rep->scaler->setAspectRatio(val);
  this->sigNodeChanged.emit();
}

void GxShapeKit::setMaxDimension(double val)
{
GVX_TRACE("GxShapeKit::setMaxDimension");

  rep->scaler->setMaxDim(val);
  this->sigNodeChanged.emit();
}

void GxShapeKit::setAlignmentMode(int val)
{
GVX_TRACE("GxShapeKit::setAlignmentMode");

  rep->aligner->setMode(val);
  this->sigNodeChanged.emit();
}

void GxShapeKit::setCenterX(double val)
{
GVX_TRACE("GxShapeKit::setCenterX");

  rep->aligner->itsCenter.x() = val;
  this->sigNodeChanged.emit();
}

void GxShapeKit::setCenterY(double val)
{
GVX_TRACE("GxShapeKit::setCenterY");

  rep->aligner->itsCenter.y() = val;
  this->sigNodeChanged.emit();
}

void GxShapeKit::setPercentBorder(int pixels)
{
GVX_TRACE("GxShapeKit::setPercentBorder");
  rep->boundsOutline->setPercentBorder(pixels);
}

void GxShapeKit::setCategory(int val)
{
GVX_TRACE("GxShapeKit::setCategory");
  rep->category = val;
}

void GxShapeKit::setRenderMode(int mode)
{
GVX_TRACE("GxShapeKit::setRenderMode");

  rep->cache->setMode(mode);
  this->sigNodeChanged.emit();
}


/////////////
// actions //
/////////////

void GxShapeKit::draw(Gfx::Canvas& canvas) const
{
GVX_TRACE("GxShapeKit::draw");
  rep->topNode->draw(canvas);
}

double GxShapeKit::getWidthFactor() const
{
GVX_TRACE("GxShapeKit::getWidthFactor");
  return rep->scaler->widthFactor();
}

void GxShapeKit::setWidthFactor(double val)
{
GVX_TRACE("GxShapeKit::setWidthFactor");
  rep->scaler->setWidthFactor(val);
}

double GxShapeKit::getHeightFactor() const
{
GVX_TRACE("GxShapeKit::getHeightFactor");
  return rep->scaler->heightFactor();
}

void GxShapeKit::setHeightFactor(double val)
{
GVX_TRACE("GxShapeKit::setHeightFactor");
  rep->scaler->setHeightFactor(val);
}
