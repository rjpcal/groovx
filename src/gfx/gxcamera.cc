///////////////////////////////////////////////////////////////////////
//
// gxcamera.cc
//
// Copyright (c) 2002-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Thu Nov 21 15:22:25 2002
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef GXCAMERA_CC_DEFINED
#define GXCAMERA_CC_DEFINED

#include "gxcamera.h"

#include "gfx/canvas.h"

#include "io/reader.h"
#include "io/writer.h"

#include "util/error.h"

#include <cmath>

#include "util/debug.h"
#include "util/trace.h"

GxCamera::~GxCamera() throw() {}

GxPerspectiveCamera::GxPerspectiveCamera() :
  GxCamera(),
  FieldContainer(&sigNodeChanged),
  itsFovY(30),
  itsNearZ(1),
  itsFarZ(30)
{}

GxPerspectiveCamera::~GxPerspectiveCamera() throw() {}

const FieldMap& GxPerspectiveCamera::classFields()
{
DOTRACE("GxPerspectiveCamera::classFields");
  static const Field FIELD_ARRAY[] =
  {
    Field("fovY", &GxPerspectiveCamera::itsFovY, 30.0, 1.0, 180.0, 1.0,
          Field::NEW_GROUP),
    Field("nearZ", &GxPerspectiveCamera::itsNearZ, 1.0, 1.0, 50.0, 0.1),
    Field("farZ", &GxPerspectiveCamera::itsFarZ, 30.0, 1.0, 50.0, 0.1),
    Field("translation", Field::ValueType(), &GxPerspectiveCamera::translation,
          "0. 0. 0.", "-50. -50. -10.", "50. 50. 50.", "0.1 0.1 0.1",
          Field::MULTI)
  };

  static FieldMap FIELD_MAP(FIELD_ARRAY);

  return FIELD_MAP;
}

void GxPerspectiveCamera::readFrom(IO::Reader& reader)
{
DOTRACE("GxPerspectiveCamera::readFrom");
  readFieldsFrom(reader, classFields());
}

void GxPerspectiveCamera::writeTo(IO::Writer& writer) const
{
DOTRACE("GxPerspectiveCamera::writeTo");
  writeFieldsTo(writer, classFields());
}

void GxPerspectiveCamera::draw(Gfx::Canvas& canvas) const
{
DOTRACE("GxPerspectiveCamera::draw");

  dbgEval(2, width()); dbgEvalNL(2, height());

  canvas.viewport(0, 0, width(), height());

  canvas.perspective(itsFovY,
                     double(width()) / double(height()),
                     itsNearZ, itsFarZ);

  canvas.translate(translation.vec());
}


GxFixedRectCamera::~GxFixedRectCamera() throw() {}

void GxFixedRectCamera::draw(Gfx::Canvas& canvas) const
{
DOTRACE("GxFixedRectCamera::draw");

  canvas.viewport(0, 0, width(), height());
  canvas.orthographic(itsRect, -10.0, 10.0);
}

GxMinRectCamera::~GxMinRectCamera() throw() {}

void GxMinRectCamera::draw(Gfx::Canvas& canvas) const
{
DOTRACE("GxMinRectCamera::draw");

  canvas.viewport(0, 0, width(), height());

  // the actual rect that we'll build:
  Gfx::Rect<double> port(itsRect);

  // the desired conditions are as follows:
  //    (1) port contains itsRect
  //    (2) port.aspect() == getAspect()
  //    (3) port is the smallest rectangle that meets (1) and (2)

  const double window_aspect = double(width()) / double(height());

  const double ratio_of_aspects = itsRect.aspect() / window_aspect;

  if ( ratio_of_aspects < 1 ) // the available space is too wide...
    {
      port.scaleX(1/ratio_of_aspects); // so use some extra width
    }
  else                        // the available space is too tall...
    {
      port.scaleY(ratio_of_aspects); // so use some extra height
    }

  canvas.orthographic(port, -10.0, 10.0);
}

namespace
{
  void orthoFixed(Gfx::Canvas& canvas, int w, int h, double ppu)
  {
    const double l = -1 * (w / 2.0) / ppu;
    const double r =      (w / 2.0) / ppu;
    const double b = -1 * (h / 2.0) / ppu;
    const double t =      (h / 2.0) / ppu;

    canvas.orthographic(Gfx::rectLTRB<double>(l, t, r, b), -10.0, 10.0);
  }
}

GxFixedScaleCamera::GxFixedScaleCamera() :
  GxCamera(),
  FieldContainer(&sigNodeChanged),
  itsPixelsPerUnit(100.0)
{}

GxFixedScaleCamera::~GxFixedScaleCamera() throw() {}

const FieldMap& GxFixedScaleCamera::classFields()
{
DOTRACE("GxFixedScaleCamera::classFields");
  static const Field FIELD_ARRAY[] =
  {
    Field("pixelsPerUnit",
          make_mypair(&GxFixedScaleCamera::getPixelsPerUnit,
                      &GxFixedScaleCamera::setPixelsPerUnit),
          2.05, 1.0, 10000.0, 1.0,
          Field::NEW_GROUP),
    Field("logPixelsPerUnit",
          make_mypair(&GxFixedScaleCamera::getLogPixelsPerUnit,
                      &GxFixedScaleCamera::setLogPixelsPerUnit),
          0.5, 0.0, 5.0, 0.025,
          Field::TRANSIENT)
  };

  static FieldMap FIELD_MAP(FIELD_ARRAY);

  return FIELD_MAP;
}

void GxFixedScaleCamera::readFrom(IO::Reader& reader)
{
DOTRACE("GxFixedScaleCamera::readFrom");
  readFieldsFrom(reader, classFields());
}

void GxFixedScaleCamera::writeTo(IO::Writer& writer) const
{
DOTRACE("GxFixedScaleCamera::writeTo");
  writeFieldsTo(writer, classFields());
}

double GxFixedScaleCamera::getLogPixelsPerUnit() const
{
  return log10(itsPixelsPerUnit);
}

void GxFixedScaleCamera::setLogPixelsPerUnit(double s)
{
  setPixelsPerUnit(pow(10.0, s));
}

void GxFixedScaleCamera::setPixelsPerUnit(double s)
{
  if (s <= 0.0)
    throw Util::Error("invalid scaling factor");

  itsPixelsPerUnit = s;
}

void GxFixedScaleCamera::draw(Gfx::Canvas& canvas) const
{
DOTRACE("GxFixedScaleCamera::draw");

  canvas.viewport(0, 0, width(), height());

  orthoFixed(canvas, width(), height(), itsPixelsPerUnit);
}

GxPsyphyCamera::GxPsyphyCamera() :
  GxCamera(),
  FieldContainer(&sigNodeChanged),
  itsDegreesPerUnit(2.05),
  itsViewingDistance(30.0)
{}

GxPsyphyCamera::~GxPsyphyCamera() throw() {}

const FieldMap& GxPsyphyCamera::classFields()
{
DOTRACE("GxPsyphyCamera::classFields");
  static const Field FIELD_ARRAY[] =
  {
    Field("unitAngle",
          make_mypair(&GxPsyphyCamera::getUnitAngle,
                      &GxPsyphyCamera::setUnitAngle),
          2.05, 0.1, 100.0, 0.1,
          Field::NEW_GROUP),
    Field("viewingDistIn",
          make_mypair(&GxPsyphyCamera::getViewingDistIn,
                      &GxPsyphyCamera::setViewingDistIn),
          30.0, 1.0, 500.0, 1.0),
  };

  static FieldMap FIELD_MAP(FIELD_ARRAY);

  return FIELD_MAP;
}

void GxPsyphyCamera::readFrom(IO::Reader& reader)
{
DOTRACE("GxPsyphyCamera::readFrom");
  readFieldsFrom(reader, classFields());
}

void GxPsyphyCamera::writeTo(IO::Writer& writer) const
{
DOTRACE("GxPsyphyCamera::writeTo");
  writeFieldsTo(writer, classFields());
}

void GxPsyphyCamera::setUnitAngle(double deg_per_unit)
{
  if (deg_per_unit <= 0.0)
    throw Util::Error("unit angle must be positive");

  itsDegreesPerUnit = deg_per_unit;
}

void GxPsyphyCamera::setViewingDistIn(double inches)
{
  if (inches <= 0.0)
    throw Util::Error("viewing distance must be positive (duh)");

  itsViewingDistance = inches;
}

void GxPsyphyCamera::draw(Gfx::Canvas& canvas) const
{
DOTRACE("GxPsyphyCamera::draw");

  canvas.viewport(0, 0, width(), height());

  static const double deg_to_rad = M_PI/180.0;

  const double pixels_per_inch = 72.0; // FIXME should get this from Canvas

  // geometry 101:
  //                   tan(angle_per_unit) == inches_per_unit/viewing_dist;
  const double inches_per_unit =
    tan(itsDegreesPerUnit*deg_to_rad) * itsViewingDistance;

  const double pixels_per_unit = pixels_per_inch * inches_per_unit;

  orthoFixed(canvas, width(), height(), pixels_per_unit);
}

static const char vcid_gxcamera_cc[] = "$Header$";
#endif // !GXCAMERA_CC_DEFINED
