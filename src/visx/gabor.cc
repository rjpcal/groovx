///////////////////////////////////////////////////////////////////////
//
// gabor.cc
//
// Copyright (c) 1999-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Wed Oct  6 10:45:58 1999
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
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

#ifndef GABOR_CC_DEFINED
#define GABOR_CC_DEFINED

#include "visx/gabor.h"

#include "geom/rect.h"
#include "geom/vec2.h"

#include "gfx/bbox.h"
#include "gfx/canvas.h"
#include "gfx/gxscaler.h"

#include "io/ioproxy.h"
#include "io/reader.h"
#include "io/writer.h"

#include "media/bmapdata.h"
#include "media/imgfile.h"

#include "util/algo.h"
#include "util/rand.h"

#include <cmath>

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER

using geom::vec2i;
using geom::vec2d;

namespace
{
  const IO::VersionId GABOR_SERIAL_VERSION_ID = 2;
}

const Gabor::ColorMode Gabor::GRAYSCALE;
const Gabor::ColorMode Gabor::COLOR_INDEX;
const Gabor::ColorMode Gabor::BW_DITHER_POINT;
const Gabor::ColorMode Gabor::BW_DITHER_RECT;

const FieldMap& Gabor::classFields()
{
  static const Field FIELD_ARRAY[] =
  {
    Field("colorMode", &Gabor::itsColorMode, 2, 1, 4, 1, Field::NEW_GROUP),
    Field("contrast", &Gabor::itsContrast, 1.0, 0.0, 1.0, 0.05),
    Field("logContrast",
          &Gabor::getLogContrast, &Gabor::setLogContrast,
          0.0, -3.0, 0.0, 0.1, Field::TRANSIENT),
    Field("spatialFreq", &Gabor::itsSpatialFreq, 3.5, 0.5, 10.0, 0.5),
    Field("phase", &Gabor::itsPhase, 0, 0, 359, 1),
    Field("drift", &Gabor::itsDrift, 0, 0, 359, 1, Field::TRANSIENT),
    Field("sigma", &Gabor::itsSigma, 0.15, 0.025, 0.5, 0.025),
    Field("aspectRatio", &Gabor::itsAspectRatio, 1.0, 0.1, 10.0, 0.1),
    Field("orientation", &Gabor::itsOrientation, 0, -180, 179, 1),
    Field("resolution", &Gabor::itsResolution, 60, 5, 500, 1),
    Field("pointSize", &Gabor::itsPointSize, 1, 1, 25, 1),
    Field("fgTint", Field::ValueType(), &Gabor::itsFgTint,
           "1.0 1.0 1.0 1.0", "0.0 0.0 0.0 0.0", "1.0 1.0 1.0 1.0",
          "0.01 0.01 0.01 0.01", Field::NEW_GROUP | Field::MULTI),
    Field("bgTint", Field::ValueType(), &Gabor::itsBgTint,
           "0.0 0.0 0.0 0.0", "0.0 0.0 0.0 0.0", "1.0 1.0 1.0 1.0",
          "0.01 0.01 0.01 0.01", Field::MULTI)
  };

  static FieldMap GABOR_FIELDS(FIELD_ARRAY, &GxShapeKit::classFields());

  return GABOR_FIELDS;
}

Gabor* Gabor::make()
{
DOTRACE("Gabor::make");
  return new Gabor;
}

Gabor::Gabor() :
  itsColorMode(1),
  itsContrast(1.0),
  itsSpatialFreq(3.5),
  itsPhase(0),
  itsDrift(0),
  itsDrawnPhase(0),
  itsSigma(0.15),
  itsAspectRatio(1.0),
  itsOrientation(0),
  itsResolution(116),
  itsPointSize(2),
  itsFgTint(1.0, 1.0, 1.0, 1.0),
  itsBgTint(0.0, 0.0, 0.0, 0.0)
{
DOTRACE("Gabor::Gabor");

  setFieldMap(Gabor::classFields());

  setScalingMode(GxScaler::MAINTAIN_ASPECT_SCALING);
}

Gabor::~Gabor () throw()
{
DOTRACE("Gabor::~Gabor");
}

IO::VersionId Gabor::serialVersionId() const
{
DOTRACE("Gabor::serialVersionId");
  return GABOR_SERIAL_VERSION_ID;
}

void Gabor::readFrom(IO::Reader& reader)
{
DOTRACE("Gabor::readFrom");

  reader.ensureReadVersionId("Gabor", 2,
                             "Try cvs tag xml_conversion_20040526",
                             SRC_POS);

  readFieldsFrom(reader, classFields());

  reader.readBaseClass("GxShapeKit", IO::makeProxy<GxShapeKit>(this));
}

void Gabor::writeTo(IO::Writer& writer) const
{
DOTRACE("Gabor::writeTo");

  writer.ensureWriteVersionId("Gabor", GABOR_SERIAL_VERSION_ID, 2,
                              "Try groovx0.8a4", SRC_POS);

  writeFieldsTo(writer, classFields());

  writer.writeBaseClass("GxShapeKit", IO::makeConstProxy<GxShapeKit>(this));
}

void Gabor::setLogContrast(double logContrast)
{
DOTRACE("Gabor::setLogContrast");

  if (logContrast <= 0.0)
    {
      itsContrast = std::pow(10.0, logContrast);
    }
}

double Gabor::getLogContrast() const
{
DOTRACE("getLogContrast");

  return (itsContrast > 0.0) ? std::log10(itsContrast) : -10.0;
}

void Gabor::grGetBoundingBox(Gfx::Bbox& bbox) const
{
DOTRACE("Gabor::grGetBoundingBox");

  const vec2d world_origin(0.0, 0.0);

  const vec2i screen_origin = vec2i(bbox.screenFromWorld2(world_origin));

  const vec2i size(itsResolution * itsPointSize,
                   itsResolution * itsPointSize);

  const geom::rect<int> screen_rect =
    geom::rect<int>().set_lbwh(screen_origin, size);

  const geom::rect<double> world_rect = bbox.worldFromScreenRect(screen_rect);

  bbox.drawRect(world_rect);
}

void Gabor::getBmapData(media::bmap_data& dest) const
{
DOTRACE("Gabor::getBmapData");
  const double xsigma2 = itsSigma*itsAspectRatio * itsSigma*itsAspectRatio;
  const double ysigma2 = itsSigma * itsSigma;

  const vec2d center(0.0, 0.0);

  static const double PI = acos(-1.0);

  if (itsDrift == 0)
    {
      itsDrawnPhase = itsPhase;
    }
  else
    {
      itsDrawnPhase += itsDrift;
      itsDrawnPhase %= 360;
    }

  //                      1            ( -(x-mean)^2 / (2*sigma)^2 )
  // gaussian(x) = ---------------- * e
  //               sigma*sqrt(2*pi)

  // For our purposes, we ignore the scaling factor out front, so that
  // the maximum value the function takes on is 1 (the scaling factor
  // is otherwise there to keep the total area under the curve to 1,
  // but this affects the maximum height).

  double res_step = 1.0/itsResolution;

  vec2i size(itsResolution, itsResolution);

  int bits_per_pixel = (itsColorMode == GRAYSCALE) ? 32 : 8;

  media::bmap_data data(size, bits_per_pixel, 1);

  typedef unsigned char ubyte;

  ubyte* bytes = data.bytes_ptr();

  ubyte* bytes_end = bytes + data.byte_count();

  for (int y_pos = 0; y_pos < itsResolution; ++y_pos)
    {
      const double unrotated_y = y_pos*res_step - 0.5;

      for (int x_pos = 0; x_pos < itsResolution; ++x_pos)
        {
          const double unrotated_x = x_pos*res_step - 0.5;

          vec2d point(unrotated_x, unrotated_y);
          point.rotate_deg(itsOrientation);

          point -= center;

          const double gauss_xy =
            exp( (point.x())*(point.x()) / (-4.0*xsigma2) +
                 (point.y())*(point.y()) / (-4.0*ysigma2) );

          const double sin_x =
            sin(2*PI*itsSpatialFreq*point.x() + itsDrawnPhase*PI/180.0);

          const double gabor = 0.5*sin_x*gauss_xy*itsContrast + 0.5;

          ASSERT( bytes < bytes_end );

          if ( itsColorMode == GRAYSCALE )
            {
              *bytes++ = ubyte
                ((itsFgTint.color().r() * gabor +
                  itsBgTint.color().r() * (1-gabor)) * 255);
              *bytes++ = ubyte
                ((itsFgTint.color().g() * gabor +
                  itsBgTint.color().g() * (1-gabor)) * 255);
              *bytes++ = ubyte
                ((itsFgTint.color().b() * gabor +
                  itsBgTint.color().b() * (1-gabor)) * 255);
              *bytes++ = ubyte
                ((itsFgTint.color().a() * gabor +
                  itsBgTint.color().a() * (1-gabor)) * 255);
            }
          else if ( itsColorMode == COLOR_INDEX )
            {
              *bytes++ = ubyte(gabor * 255);
            }
          else if ( itsColorMode == BW_DITHER_POINT ||
                    itsColorMode == BW_DITHER_RECT )
            {
              *bytes++ = (rutz::rand_range(0.0, 1.0) < gabor) ? 255 : 0;
            }
        }
    }

  data.swap(dest);
}

void Gabor::saveImage(const char* filename) const
{
DOTRACE("Gabor::saveImage");

  media::bmap_data data;
  getBmapData(data);

  media::save_image(filename, data);
}

void Gabor::grRender(Gfx::Canvas& canvas) const
{
DOTRACE("Gabor::grRender");

  media::bmap_data data;
  getBmapData(data);

  canvas.drawPixels(data, vec2d(0.0, 0.0),
                    vec2d(itsPointSize, itsPointSize));
}

static const char vcid_gabor_cc[] = "$Header$";
#endif // !GABOR_CC_DEFINED
