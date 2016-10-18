/** @file gfx/gxcamera.cc view a graphic scene with orthographic or
    perspective projection */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Thu Nov 21 15:22:25 2002
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

#ifndef GROOVX_GFX_GXCAMERA_CC_UTC20050626084024_DEFINED
#define GROOVX_GFX_GXCAMERA_CC_UTC20050626084024_DEFINED

#include "gxcamera.h"

#include "gfx/canvas.h"

#include "io/reader.h"
#include "io/writer.h"

#include "rutz/error.h"

#include <cmath>

#include "rutz/debug.h"
GVX_DBG_REGISTER
#include "rutz/trace.h"

namespace
{
  const int GXPC_SVID = 0;
  const int GXFSC_SVID = 0;
  const int GXPPC_SVID = 0;
}

GxCamera::~GxCamera() noexcept {}

void GxCamera::reshape(Gfx::Canvas& canvas, int w, int h)
{
GVX_TRACE("GxCamera::reshape");

  dbg_eval(3, itsWidth); dbg_eval_nl(3, itsHeight);

  itsWidth = w; itsHeight = h;

  dbg_eval(3, itsWidth); dbg_eval_nl(3, itsHeight);

  // NOTE: We must call draw() here in order to make sure that e.g. the
  // right viewport() gets set. Without the draw() call, the potential
  // problem is that the viewport only gets updated inside a redraw
  // sequence. Since GxScene::render() wraps such a sequence inside a
  // MatrixSaver, any change to the viewport will be lost following the
  // redraw sequence. Then we run into problems if e.g. we try to grab
  // pixels from the current viewport into a GxPixmap, since the viewport
  // doesn't reflect the full window size.
  draw(canvas);
}

GxPerspectiveCamera::GxPerspectiveCamera() :
  GxCamera(),
  FieldContainer(&sigNodeChanged),
  itsFovY(30),
  itsNearZ(1),
  itsFarZ(30)
{}

GxPerspectiveCamera::~GxPerspectiveCamera() noexcept {}

const FieldMap& GxPerspectiveCamera::classFields()
{
GVX_TRACE("GxPerspectiveCamera::classFields");
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

io::version_id GxPerspectiveCamera::class_version_id() const
{
GVX_TRACE("GxPerspectiveCamera::class_version_id");
  return GXPC_SVID;
}

void GxPerspectiveCamera::read_from(io::reader& reader)
{
GVX_TRACE("GxPerspectiveCamera::read_from");
  readFieldsFrom(reader, classFields());
}

void GxPerspectiveCamera::write_to(io::writer& writer) const
{
GVX_TRACE("GxPerspectiveCamera::write_to");
  writeFieldsTo(writer, classFields(), GXPC_SVID);
}

void GxPerspectiveCamera::draw(Gfx::Canvas& canvas) const
{
GVX_TRACE("GxPerspectiveCamera::draw");

  dbg_eval(2, width()); dbg_eval_nl(2, height());

  canvas.viewport(0, 0, width(), height());

  canvas.perspective(itsFovY,
                     double(width()) / double(height()),
                     itsNearZ, itsFarZ);

  canvas.translate(translation.vec());
}


GxFixedRectCamera::~GxFixedRectCamera() noexcept {}

void GxFixedRectCamera::draw(Gfx::Canvas& canvas) const
{
GVX_TRACE("GxFixedRectCamera::draw");

  canvas.viewport(0, 0, width(), height());
  canvas.orthographic(itsRect, -10.0, 10.0);
}

GxMinRectCamera::~GxMinRectCamera() noexcept {}

void GxMinRectCamera::draw(Gfx::Canvas& canvas) const
{
GVX_TRACE("GxMinRectCamera::draw");

  canvas.viewport(0, 0, width(), height());

  // the actual rect that we'll build:
  geom::rect<double> port(itsRect);

  // the desired conditions are as follows:
  //    (1) port contains itsRect
  //    (2) port.aspect() == getAspect()
  //    (3) port is the smallest rectangle that meets (1) and (2)

  const double window_aspect = double(width()) / double(height());

  const double ratio_of_aspects = itsRect.aspect() / window_aspect;

  if ( ratio_of_aspects < 1 ) // the available space is too wide...
    {
      port.scale_x(1/ratio_of_aspects); // so use some extra width
    }
  else                        // the available space is too tall...
    {
      port.scale_y(ratio_of_aspects); // so use some extra height
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

    canvas.orthographic(geom::rect<double>::ltrb(l,t,r,b), -10.0, 10.0);
  }
}

GxFixedScaleCamera::GxFixedScaleCamera() :
  GxCamera(),
  FieldContainer(&sigNodeChanged),
  itsPixelsPerUnit(100.0)
{}

GxFixedScaleCamera::~GxFixedScaleCamera() noexcept {}

const FieldMap& GxFixedScaleCamera::classFields()
{
GVX_TRACE("GxFixedScaleCamera::classFields");
  static const Field FIELD_ARRAY[] =
  {
    Field("pixelsPerUnit",
          &GxFixedScaleCamera::getPixelsPerUnit,
          &GxFixedScaleCamera::setPixelsPerUnit,
          2.05, 1.0, 10000.0, 1.0,
          Field::NEW_GROUP),
    Field("logPixelsPerUnit",
          &GxFixedScaleCamera::getLogPixelsPerUnit,
          &GxFixedScaleCamera::setLogPixelsPerUnit,
          0.5, 0.0, 5.0, 0.025,
          Field::TRANSIENT)
  };

  static FieldMap FIELD_MAP(FIELD_ARRAY);

  return FIELD_MAP;
}

io::version_id GxFixedScaleCamera::class_version_id() const
{
GVX_TRACE("GxFixedScaleCamera::class_version_id");
  return GXFSC_SVID;
}

void GxFixedScaleCamera::read_from(io::reader& reader)
{
GVX_TRACE("GxFixedScaleCamera::read_from");
  readFieldsFrom(reader, classFields());
}

void GxFixedScaleCamera::write_to(io::writer& writer) const
{
GVX_TRACE("GxFixedScaleCamera::write_to");
  writeFieldsTo(writer, classFields(), GXFSC_SVID);
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
    throw rutz::error("invalid scaling factor", SRC_POS);

  itsPixelsPerUnit = s;
}

void GxFixedScaleCamera::draw(Gfx::Canvas& canvas) const
{
GVX_TRACE("GxFixedScaleCamera::draw");

  dbg_eval(3, width()); dbg_eval_nl(3, height());
  canvas.viewport(0, 0, width(), height());

  orthoFixed(canvas, width(), height(), itsPixelsPerUnit);
}

GxPsyphyCamera::GxPsyphyCamera() :
  GxCamera(),
  FieldContainer(&sigNodeChanged),
  itsDegreesPerUnit(2.05),
  itsViewingDistance(30.0)
{}

GxPsyphyCamera::~GxPsyphyCamera() noexcept {}

const FieldMap& GxPsyphyCamera::classFields()
{
GVX_TRACE("GxPsyphyCamera::classFields");
  static const Field FIELD_ARRAY[] =
  {
    Field("unitAngle",
          &GxPsyphyCamera::getUnitAngle,
          &GxPsyphyCamera::setUnitAngle,
          2.05, 0.1, 100.0, 0.1,
          Field::NEW_GROUP),
    Field("viewingDistIn",
          &GxPsyphyCamera::getViewingDistIn,
          &GxPsyphyCamera::setViewingDistIn,
          30.0, 1.0, 500.0, 1.0),
  };

  static FieldMap FIELD_MAP(FIELD_ARRAY);

  return FIELD_MAP;
}

io::version_id GxPsyphyCamera::class_version_id() const
{
GVX_TRACE("GxPsyphyCamera::class_version_id");
  return GXPPC_SVID;
}

void GxPsyphyCamera::read_from(io::reader& reader)
{
GVX_TRACE("GxPsyphyCamera::read_from");
  readFieldsFrom(reader, classFields());
}

void GxPsyphyCamera::write_to(io::writer& writer) const
{
GVX_TRACE("GxPsyphyCamera::write_to");
  writeFieldsTo(writer, classFields(), GXPPC_SVID);
}

void GxPsyphyCamera::setUnitAngle(double deg_per_unit)
{
  if (deg_per_unit <= 0.0)
    throw rutz::error("unit angle must be positive", SRC_POS);

  itsDegreesPerUnit = deg_per_unit;
}

void GxPsyphyCamera::setViewingDistIn(double inches)
{
  if (inches <= 0.0)
    throw rutz::error("viewing distance must be positive (duh)", SRC_POS);

  itsViewingDistance = inches;
}

void GxPsyphyCamera::draw(Gfx::Canvas& canvas) const
{
GVX_TRACE("GxPsyphyCamera::draw");

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

#endif // !GROOVX_GFX_GXCAMERA_CC_UTC20050626084024_DEFINED
