///////////////////////////////////////////////////////////////////////
//
// gxtransform.cc
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Nov 20 15:15:50 2002
// written: Wed Nov 20 15:16:47 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXTRANSFORM_CC_DEFINED
#define GXTRANSFORM_CC_DEFINED

#include "gxtransform.h"

#include "gfx/canvas.h"

#include "gx/bbox.h"
#include "gx/box.h"
#include "gx/rect.h"
#include "gx/txform.h"
#include "gx/vec3.h"

#include "io/reader.h"
#include "io/writer.h"

#include "util/trace.h"
#include "util/debug.h"

namespace
{
  const IO::VersionId POS_SERIAL_VERSION_ID = 1;
}

///////////////////////////////////////////////////////////////////////
//
// GxTransformImpl structure
//
///////////////////////////////////////////////////////////////////////

struct GxTransformImpl
{
  GxTransformImpl(GxTransform* p) :
    owner(p),
    txformDirty(true),
    txformCache()
  {}

  GxTransform* owner;

  mutable bool txformDirty;
private:
  mutable Gfx::Txform txformCache;

public:
  const Gfx::Txform& getTxform() const
  {
    if (txformDirty)
      {
        txformCache = Gfx::Txform(owner->translation,
                                  owner->scaling,
                                  owner->rotationAxis,
                                  owner->itsRotationAngle);
        txformDirty = false;
      }

    return txformCache;
  }
};

///////////////////////////////////////////////////////////////////////
//
// GxTransform member functions
//
///////////////////////////////////////////////////////////////////////

const FieldMap& GxTransform::classFields()
{
  static const Field FIELD_ARRAY[] =
  {
    Field("translation", Field::ValueType(), &GxTransform::translation,
          "0. 0. 0.", "-10. -10. -10.", "10. 10. 10.", "0.1 0.1 0.1",
          Field::NEW_GROUP | Field::MULTI),
    Field("scaling", Field::ValueType(), &GxTransform::scaling,
          "1. 1. 1.", "0.1 0.1 0.1", "10. 10. 10.", "0.1 0.1 0.1",
          Field::MULTI),
    Field("rotationAxis", Field::ValueType(), &GxTransform::rotationAxis,
          "0. 0. 1.", "-1. -1. -1.", "1. 1. 1.", "0.1 0.1 0.1", Field::MULTI),
    Field("rotationAngle",
          &GxTransform::itsRotationAngle, 0., 0., 360., 1.),
    Field("mtxMode", &GxTransform::itsMtxMode, 0, 0, 1, 1,
          Field::TRANSIENT | Field::BOOLEAN),
    Field("jackSize", &GxTransform::itsJackSize, 0, 0, 100, 1, Field::TRANSIENT)
  };

  static FieldMap POS_FIELDS(FIELD_ARRAY);

  return POS_FIELDS;
}

GxTransform* GxTransform::make()
{
DOTRACE("GxTransform::make");
  return new GxTransform;
}

GxTransform::GxTransform() :
  FieldContainer(&sigNodeChanged),
  translation(0.0, 0.0, 0.0),
  scaling(1.0, 1.0, 1.0),
  rotationAxis(0.0, 0.0, 1.0),
  itsRotationAngle(0.0),
  itsMtxMode(0),
  itsJackSize(0),
  rep(new GxTransformImpl(this))
{
DOTRACE("GxTransform::GxTransform");

  dbgEvalNL(3, (void *) rep);

  setFieldMap(GxTransform::classFields());

  sigNodeChanged.connect(this, &GxTransform::onChange);
}

GxTransform::~GxTransform()
{
DOTRACE("GxTransform::~GxTransform");
  delete rep;
}

IO::VersionId GxTransform::serialVersionId() const
{
DOTRACE("GxTransform::serialVersionId");
  return POS_SERIAL_VERSION_ID;
}

void GxTransform::readFrom(IO::Reader* reader)
{
DOTRACE("GxTransform::readFrom");

  IO::VersionId svid = reader->readSerialVersionId();
  if (svid == 0)
    {
      reader->readValue("transX", translation.x());
      reader->readValue("transY", translation.y());
      reader->readValue("transZ", translation.z());

      reader->readValue("scaleX", scaling.x());
      reader->readValue("scaleY", scaling.y());
      reader->readValue("scaleZ", scaling.z());

      reader->readValue("rotateX", rotationAxis.x());
      reader->readValue("rotateY", rotationAxis.y());
      reader->readValue("rotateZ", rotationAxis.z());
      reader->readValue("rotateAngle", itsRotationAngle);
    }
  else
    {
      reader->ensureReadVersionId("GxTransform", 1, "Try grsh0.8a4");

      readFieldsFrom(reader, classFields());
    }
}

void GxTransform::writeTo(IO::Writer* writer) const
{
DOTRACE("GxTransform::writeTo");

  writer->ensureWriteVersionId("GxTransform", POS_SERIAL_VERSION_ID, 1,
                               "Try grsh0.8a4");

  writeFieldsTo(writer, classFields());
}

///////////////
// accessors //
///////////////

const Gfx::Txform& GxTransform::getTxform() const
{
DOTRACE("GxTransform::getTxform");

  return rep->getTxform();
}

/////////////
// actions //
/////////////

void GxTransform::getBoundingCube(Gfx::Bbox& bbox) const
{
DOTRACE("GxTransform::getBoundingCube");

  bbox.transform(getTxform());
}

void GxTransform::draw(Gfx::Canvas& canvas) const
{
DOTRACE("GxTransform::draw");

  if (itsMtxMode == 0)
    {
      DOTRACE("GxTransform::draw::native-transform");
      canvas.translate(translation);
      canvas.scale(scaling);
      canvas.rotate(rotationAxis, itsRotationAngle);
    }
  else
    {
      DOTRACE("GxTransform::draw::custom-transform");
      canvas.transform(rep->getTxform());
    }

  if (itsJackSize > 0)
    {
      Gfx::LinesBlock block(canvas);

      const double d = itsJackSize / 10.0;

      canvas.vertex3(Gfx::Vec3<double>(-d, 0.0, 0.0));
      canvas.vertex3(Gfx::Vec3<double>( d, 0.0, 0.0));

      canvas.vertex3(Gfx::Vec3<double>(0.0, -d, 0.0));
      canvas.vertex3(Gfx::Vec3<double>(0.0,  d, 0.0));

      canvas.vertex3(Gfx::Vec3<double>(0.0, 0.0, -d));
      canvas.vertex3(Gfx::Vec3<double>(0.0, 0.0,  d));
    }
}

void GxTransform::onChange()
{
  rep->txformDirty = true;
}

static const char vcid_gxtransform_cc[] = "$Header$";
#endif // !GXTRANSFORM_CC_DEFINED
