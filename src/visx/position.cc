///////////////////////////////////////////////////////////////////////
//
// position.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Mar 10 21:33:15 1999
// written: Fri Jun 21 13:31:12 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef POSITION_CC_DEFINED
#define POSITION_CC_DEFINED

#include "visx/position.h"

#include "gfx/canvas.h"

#include "gx/vec3.h"

#include "io/reader.h"
#include "io/writer.h"

#include "util/algo.h"

#include <cmath>
#include <GL/gl.h>

#include "util/trace.h"
#include "util/debug.h"

#ifdef PPC
#include "gfx/gbvec.cc"

void instant_gbvec()
{
  GbVec3<int> pi; pi.~GbVec3<int>();
  GbVec3<double> pd; pd.~GbVec3<double>();
}
#endif

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
          &Position::itsRotationAngle, 0., 0., 360., 1.),
    Field("mtxMode", &Position::itsMtxMode, 0, 0, 1, 1,
          Field::TRANSIENT | Field::BOOLEAN)
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

///////////////
// accessors //
///////////////

namespace
{
  void mul_mtx_4x4(const double* m1, const double* m2, double* result)
  {
    // Note we're using column-major order here
    for (int result_col = 0; result_col < 4; ++result_col)
      for (int result_row = 0; result_row < 4; ++result_row)
        {
          result[result_col * 4 + result_row] =
              m1[0 * 4 + result_row] * m2[result_col * 4 + 0]
            + m1[1 * 4 + result_row] * m2[result_col * 4 + 1]
            + m1[2 * 4 + result_row] * m2[result_col * 4 + 2]
            + m1[3 * 4 + result_row] * m2[result_col * 4 + 3];
        }
  }
}

void Position::getTransformation(double* result) const
{
DOTRACE("Position::getTransformation");

  // The matrices look transposed because OpenGL expects column-major

  // The result of (1) translation, then (2) scaling
  /*
  const double ts[16] =
    {
      scaling.x(),     0.0,             0.0,                     0.0,
      0.0,             scaling.y(),     0.0,                     0.0,
      0.0,             0.0,             scaling.z(),             0.0,
      translation.x(), translation.y(), translation.z(),         1.0
    };
  */

  const double rl = rotationAxis.length();

  const double rx = rotationAxis.x() / rl;
  const double ry = rotationAxis.y() / rl;
  const double rz = rotationAxis.z() / rl;

  const double c = cos((itsRotationAngle) * M_PI / 180.0);
  const double s = sin((itsRotationAngle) * M_PI / 180.0);

  // The result of rotation
  /*
  const double r[16] =
    {
      rx*rx*(1-c)+c,    ry*rx*(1-c)+rz*s, rx*rz*(1-c)-ry*s, 0.0,
      rx*ry*(1-c)-rz*s, ry*ry*(1-c)+c,    ry*rz*(1-c)+rx*s, 0.0,
      rx*rz*(1-c)+ry*s, ry*rz*(1-c)-rx*s, rz*rz*(1-c)+c,    0.0,
      0.0,              0.0,              0.0,              1.0
    };
  */

  const double sx = scaling.x();
  const double sy = scaling.y();
  const double sz = scaling.z();

  const double tx = translation.x();
  const double ty = translation.y();
  const double tz = translation.z();

  const double tsr[16] =
    {
      sx*(rx*rx*(1-c)+c),    sy*(ry*rx*(1-c)+rz*s), sz*(rz*rx*(1-c)-ry*s), 0.0,
      sx*(rx*ry*(1-c)-rz*s), sy*(ry*ry*(1-c)+c),    sz*(rz*ry*(1-c)+rx*s), 0.0,
      sx*(rx*rz*(1-c)+ry*s), sy*(ry*rz*(1-c)-rx*s), sz*(rz*rz*(1-c)+c),    0.0,
      tx,                    ty,                    tz,                    1.0
    };

  for (int i = 0; i < 16; ++i)
    result[i] = tsr[i];

  // Compute result = t * s * r = ts * r;
  /* mul_mtx_4x4(ts, r, result); */
}

/////////////
// actions //
/////////////

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
      double mtx[16];
      getTransformation(mtx);
      glLoadMatrixd(mtx);
    }

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
