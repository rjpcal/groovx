///////////////////////////////////////////////////////////////////////
//
// position.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Mar 10 21:33:15 1999
// written: Thu Aug 30 16:54:52 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef POSITION_CC_DEFINED
#define POSITION_CC_DEFINED

#include "position.h"

#include "gfx/canvas.h"
#include "gfx/vec3.h"

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
  PositionImpl() :
    tr(0.0, 0.0, 0.0),
    sc(1.0, 1.0, 1.0),
    rt(0.0, 0.0, 1.0),
    rt_ang(0.0)
  {}

  Gfx::Vec3<double> tr;         // x,y,z coord shift
  Gfx::Vec3<double> sc;         // x,y,z scaling
  Gfx::Vec3<double> rt;         // vector of rotation axis
  double rt_ang;                // angle in degrees of rotation around axis
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
          &Position::itsRotationAngle, 0., 0., 360., 1.)
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
  itsImpl(new PositionImpl)
{
DOTRACE("Position::Position()");

  DebugEvalNL((void *) itsImpl);

  setFieldMap(Position::classFields());
}

Position::~Position()
{
DOTRACE("Position::~Position");
  delete itsImpl;
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

/////////////
// actions //
/////////////

void Position::draw(Gfx::Canvas& canvas) const
{
DOTRACE("Position::draw");
  canvas.translate(translation);
  canvas.scale(scaling);
  canvas.rotate(rotationAxis, itsRotationAngle);

  itsImpl->tr = translation;
  itsImpl->sc = scaling;
  itsImpl->rt = rotationAxis;
  itsImpl->rt_ang = itsRotationAngle;
}

void Position::undraw(Gfx::Canvas& canvas) const
{
DOTRACE("Position::undraw");
  canvas.translate(itsImpl->tr);
  canvas.scale(itsImpl->sc);
  canvas.rotate(itsImpl->rt, itsImpl->rt_ang);
}

static const char vcid_position_cc[] = "$Header$";
#endif // !POSITION_CC_DEFINED
