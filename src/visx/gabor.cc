///////////////////////////////////////////////////////////////////////
//
// gabor.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Oct  6 10:45:58 1999
// written: Thu Aug 30 10:17:57 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GABOR_CC_DEFINED
#define GABOR_CC_DEFINED

#include "gabor.h"

#include "application.h"

#include "gfx/bmapdata.h"
#include "gfx/canvas.h"
#include "gfx/rect.h"
#include "gfx/vec2.h"

#include "io/ioproxy.h"
#include "io/reader.h"
#include "io/writer.h"

#include "util/algo.h"
#include "util/randutils.h"

#include <cmath>

#include "util/trace.h"
#include "util/debug.h"

namespace
{
  const IO::VersionId GABOR_SERIAL_VERSION_ID = 1;
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
    Field("spatialFreq", &Gabor::itsSpatialFreq, 3.5, 0.5, 10.0, 0.5),
    Field("phase", &Gabor::itsPhase, 0, -180, 179, 1),
    Field("sigma", &Gabor::itsSigma, 0.15, 0.025, 0.5, 0.025),
    Field("aspectRatio", &Gabor::itsAspectRatio, 1.0, 0.1, 10.0, 0.1),
    Field("orientation", &Gabor::itsOrientation, 0, -180, 179, 1),
    Field("resolution", &Gabor::itsResolution, 60, 5, 500, 1),
    Field("pointSize", &Gabor::itsPointSize, 1, 1, 25, 1)
  };

  static FieldMap GABOR_FIELDS(FIELD_ARRAY, &GrObj::classFields());

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
  itsSigma(0.15),
  itsAspectRatio(1.0),
  itsOrientation(0),
  itsResolution(116),
  itsPointSize(2)
{
DOTRACE("Gabor::Gabor");

  setFieldMap(Gabor::classFields());

  setScalingMode(Gmodes::MAINTAIN_ASPECT_SCALING);
}

Gabor::~Gabor ()
{
DOTRACE("Gabor::~Gabor");
}

IO::VersionId Gabor::serialVersionId() const
{
DOTRACE("Gabor::serialVersionId");
  return GABOR_SERIAL_VERSION_ID;
}

void Gabor::readFrom(IO::Reader* reader)
{
DOTRACE("Gabor::readFrom");

  reader->ensureReadVersionId("Gabor", 1, "Try grsh0.8a4");

  readFieldsFrom(reader, classFields());

  reader->readBaseClass("GrObj", IO::makeProxy<GrObj>(this));
}

void Gabor::writeTo(IO::Writer* writer) const
{
DOTRACE("Gabor::writeTo");

  writer->ensureWriteVersionId("Gabor", GABOR_SERIAL_VERSION_ID, 1,
                               "Try grsh0.8a4");

  writeFieldsTo(writer, classFields());

  writer->writeBaseClass("GrObj", IO::makeConstProxy<GrObj>(this));
}

Gfx::Rect<double> Gabor::grGetBoundingBox() const
{
DOTRACE("Gabor::grGetBoundingBox");

  Gfx::Canvas& canvas = Application::theApp().getCanvas();
  Gfx::Vec2<double> world_origin;

  Gfx::Vec2<int> screen_origin = canvas.screenFromWorld(world_origin);

  Gfx::Vec2<int> size(itsResolution * itsPointSize,
                      itsResolution * itsPointSize);

  Gfx::Rect<int> screen_rect;
  screen_rect.setRectXYWH(screen_origin.x(), screen_origin.y(),
                          size.x(), size.y());

  return canvas.worldFromScreen(screen_rect);
}

void Gabor::grRender(Gfx::Canvas& canvas) const
{
DOTRACE("Gabor::grRender");
  const double xsigma2 = itsSigma*itsAspectRatio * itsSigma*itsAspectRatio ;
  const double ysigma2 = itsSigma * itsSigma;

  const Gfx::Vec2<double> center(0.0, 0.0);

  static const double PI = acos(-1.0);

  //                      1            ( -(x-mean)^2 / (2*sigma)^2 )
  // gaussian(x) = ---------------- * e
  //               sigma*sqrt(2*pi)

  // For our purposes, we ignore the scaling factor out front, so that
  // the maximum value the function takes on is 1 (the scaling factor
  // is otherwise there to keep the total area under the curve to 1,
  // but this affects the maximum height).

  double res_step = 1.0/itsResolution;

  Gfx::Vec2<int> size(itsResolution, itsResolution);

  Gfx::BmapData data(size, 8, 1);

  unsigned char* bytes = data.bytesPtr();

  unsigned char* bytes_end = bytes + data.byteCount();

  for (int y_pos = 0; y_pos < itsResolution; ++y_pos)
    {
      const double unrotated_y = y_pos*res_step - 0.5;

      for (int x_pos = 0; x_pos < itsResolution; ++x_pos)
        {
          const double unrotated_x = x_pos*res_step - 0.5;

          Gfx::Vec2<double> point(unrotated_x, unrotated_y);
          point.rotateDeg(itsOrientation);

          point -= center;

          const double gauss_xy =
            exp( (point.x())*(point.x()) / (-4.0*xsigma2) +
                 (point.y())*(point.y()) / (-4.0*ysigma2) );

          const double sin_x =
            sin(2*PI*itsSpatialFreq*point.x() + itsPhase*PI/180.0);

          const double gabor = 0.5*sin_x*gauss_xy*itsContrast + 0.5;

          Assert( bytes < bytes_end );

          if ( itsColorMode == GRAYSCALE ||
               itsColorMode == COLOR_INDEX )
            {
              *bytes++ = (unsigned char) (gabor * 255);
            }
          else if ( itsColorMode == BW_DITHER_POINT ||
                    itsColorMode == BW_DITHER_RECT )
            {
              *bytes++ = (Util::randDoubleRange(0.0, 1.0) < gabor) ? 255 : 0;
            }
        }
    }

  canvas.drawPixels(data, Gfx::Vec2<double>(0.0, 0.0),
                    Gfx::Vec2<double>(itsPointSize, itsPointSize));
}

static const char vcid_gabor_cc[] = "$Header$";
#endif // !GABOR_CC_DEFINED
