///////////////////////////////////////////////////////////////////////
//
// position.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Mar 10 21:33:15 1999
// written: Tue Nov 19 12:53:52 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef POSITION_CC_DEFINED
#define POSITION_CC_DEFINED

#include "visx/position.h"

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
// PositionImpl structure
//
///////////////////////////////////////////////////////////////////////

struct PositionImpl
{
  PositionImpl(Position* p) :
    owner(p),
    tr(0.0, 0.0, 0.0),
    sc(1.0, 1.0, 1.0),
    rt(0.0, 0.0, 1.0),
    rt_ang(0.0),
    txformDirty(true),
    txformCache()
  {}

  Position* owner;

  Gfx::Vec3<double> tr;         // x,y,z coord shift
  Gfx::Vec3<double> sc;         // x,y,z scaling
  Gfx::Vec3<double> rt;         // vector of rotation axis
  double rt_ang;                // angle in degrees of rotation around axis

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
// Position member functions
//
///////////////////////////////////////////////////////////////////////

const FieldMap& Position::classFields()
{
  static const Field FIELD_ARRAY[] =
  {
    Field("translation", Field::ValueType(), &Position::translation,
          "0. 0. 0.", "-10. -10. -10.", "10. 10. 10.", "0.1 0.1 0.1",
          Field::NEW_GROUP | Field::MULTI),
    Field("scaling", Field::ValueType(), &Position::scaling,
          "1. 1. 1.", "0.1 0.1 0.1", "10. 10. 10.", "0.1 0.1 0.1",
          Field::MULTI),
    Field("rotationAxis", Field::ValueType(), &Position::rotationAxis,
          "0. 0. 1.", "-1. -1. -1.", "1. 1. 1.", "0.1 0.1 0.1", Field::MULTI),
    Field("rotationAngle",
          &Position::itsRotationAngle, 0., 0., 360., 1.),
    Field("mtxMode", &Position::itsMtxMode, 0, 0, 1, 1,
          Field::TRANSIENT | Field::BOOLEAN),
    Field("jackSize", &Position::itsJackSize, 0, 0, 100, 1, Field::TRANSIENT)
  };

  static FieldMap POS_FIELDS(FIELD_ARRAY);

  return POS_FIELDS;
}

Position* Position::make()
{
DOTRACE("Position::make");
  return new Position;
}

Position::Position() :
  FieldContainer(&sigNodeChanged),
  translation(0.0, 0.0, 0.0),
  scaling(1.0, 1.0, 1.0),
  rotationAxis(0.0, 0.0, 1.0),
  itsRotationAngle(0.0),
  itsMtxMode(0),
  itsJackSize(0),
  rep(new PositionImpl(this))
{
DOTRACE("Position::Position()");

  dbgEvalNL(3, (void *) rep);

  setFieldMap(Position::classFields());

  sigNodeChanged.connect(this, &Position::onChange);
}

Position::~Position()
{
DOTRACE("Position::~Position");
  delete rep;
}

IO::VersionId Position::serialVersionId() const
{
DOTRACE("Position::serialVersionId");
  return POS_SERIAL_VERSION_ID;
}

void Position::readFrom(IO::Reader* reader)
{
DOTRACE("Position::readFrom");

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
      reader->ensureReadVersionId("Position", 1, "Try grsh0.8a4");

      readFieldsFrom(reader, classFields());
    }
}

void Position::writeTo(IO::Writer* writer) const
{
DOTRACE("Position::writeTo");

  writer->ensureWriteVersionId("Position", POS_SERIAL_VERSION_ID, 1,
                               "Try grsh0.8a4");

  writeFieldsTo(writer, classFields());
}

///////////////
// accessors //
///////////////

const Gfx::Txform& Position::getTxform() const
{
DOTRACE("Position::getTxform");

  return rep->getTxform();
}

/////////////
// actions //
/////////////

void Position::getBoundingCube(Gfx::Bbox& bbox) const
{
DOTRACE("Position::getBoundingCube");

  const Gfx::Txform& txform = getTxform();

  Gfx::Vec3<double> p1 = txform.applyTo(bbox.cube.point000());
  Gfx::Vec3<double> p2 = txform.applyTo(bbox.cube.point001());
  Gfx::Vec3<double> p3 = txform.applyTo(bbox.cube.point010());
  Gfx::Vec3<double> p4 = txform.applyTo(bbox.cube.point011());
  Gfx::Vec3<double> p5 = txform.applyTo(bbox.cube.point100());
  Gfx::Vec3<double> p6 = txform.applyTo(bbox.cube.point101());
  Gfx::Vec3<double> p7 = txform.applyTo(bbox.cube.point110());
  Gfx::Vec3<double> p8 = txform.applyTo(bbox.cube.point111());

  using Util::min;
  using Util::max;

  const double x0 = min(min(min(p1.x(), p2.x()), min(p3.x(), p4.x())),
                        min(min(p5.x(), p6.x()), min(p7.x(), p8.x())));
  const double x1 = max(max(max(p1.x(), p2.x()), max(p3.x(), p4.x())),
                        max(max(p5.x(), p6.x()), max(p7.x(), p8.x())));

  const double y0 = min(min(min(p1.y(), p2.y()), min(p3.y(), p4.y())),
                        min(min(p5.y(), p6.y()), min(p7.y(), p8.y())));
  const double y1 = max(max(max(p1.y(), p2.y()), max(p3.y(), p4.y())),
                        max(max(p5.y(), p6.y()), max(p7.y(), p8.y())));

  const double z0 = min(min(min(p1.z(), p2.z()), min(p3.z(), p4.z())),
                        min(min(p5.z(), p6.z()), min(p7.z(), p8.z())));
  const double z1 = max(max(max(p1.z(), p2.z()), max(p3.z(), p4.z())),
                        max(max(p5.z(), p6.z()), max(p7.z(), p8.z())));

  bbox.cube.setXXYYZZ(x0, x1, y0, y1, z0, z1);
}

void Position::draw(Gfx::Canvas& canvas) const
{
DOTRACE("Position::draw");

  if (itsMtxMode == 0)
    {
      DOTRACE("Position::draw::native-transform");
      canvas.translate(translation);
      canvas.scale(scaling);
      canvas.rotate(rotationAxis, itsRotationAngle);
    }
  else
    {
      DOTRACE("Position::draw::custom-transform");
      canvas.transform(rep->getTxform());
    }

  rep->tr = translation;
  rep->sc = scaling;
  rep->rt = rotationAxis;
  rep->rt_ang = itsRotationAngle;

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

void Position::onChange()
{
  rep->txformDirty = true;
}

static const char vcid_position_cc[] = "$Header$";
#endif // !POSITION_CC_DEFINED
