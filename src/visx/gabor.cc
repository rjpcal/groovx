///////////////////////////////////////////////////////////////////////
//
// gabor.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Oct  6 10:45:58 1999
// written: Thu Aug  9 14:03:04 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GABOR_CC_DEFINED
#define GABOR_CC_DEFINED

#include "gabor.h"

#include "rect.h"

#include "io/ioproxy.h"
#include "io/reader.h"
#include "io/writer.h"

#include "util/algo.h"
#include "util/randutils.h"

#include <GL/gl.h>
#include <cmath>
#include <cstring>

#include "util/trace.h"
#include "util/debug.h"


namespace
{
  const FieldInfo FINFOS[] =
  {
    FieldInfo("colorMode", &Gabor::colorMode, 2, 1, 3, 1, true),
    FieldInfo("contrast", &Gabor::contrast, 1.0, 0.0, 1.0, 0.05),
    FieldInfo("spatialFreq", &Gabor::spatialFreq, 3.5, 0.5, 10.0, 0.5),
    FieldInfo("phase", &Gabor::phase, 0.0, -180.0, 179.0, 1.0),
    FieldInfo("sigma", &Gabor::sigma, 0.15, 0.025, 0.5, 0.025),
    FieldInfo("aspectRatio", &Gabor::aspectRatio, 1.0, 0.1, 10.0, 0.1),
    FieldInfo("orientation", &Gabor::orientation, 0.0, -180.0, 179.0, 1.0),
    FieldInfo("resolution", &Gabor::resolution, 60, 5, 500, 1),
    FieldInfo("pointSize", &Gabor::pointSize, 1, 1, 25, 1)
  };

  const unsigned int NUM_FINFOS = sizeof(FINFOS)/sizeof(FieldInfo);

  const FieldMap GABOR_FIELDS(FINFOS, FINFOS+NUM_FINFOS);

  const IO::VersionId GABOR_SERIAL_VERSION_ID = 1;
}

const Gabor::ColorMode Gabor::GRAYSCALE;
const Gabor::ColorMode Gabor::COLOR_INDEX;
const Gabor::ColorMode Gabor::BW_DITHER_POINT;
const Gabor::ColorMode Gabor::BW_DITHER_RECT;

const FieldMap& Gabor::classFields() { return GABOR_FIELDS; }

Gabor* Gabor::make()
{
DOTRACE("Gabor::make");
  return new Gabor;
}

Gabor::Gabor() :
  FieldContainer(),
  colorMode(1),
  contrast(1.0),
  spatialFreq(3.5),
  phase(0),
  sigma(0.15),
  aspectRatio(1.0),
  orientation(0),
  resolution(116),
  pointSize(2)
{
DOTRACE("Gabor::Gabor");

  setFieldMap(GABOR_FIELDS);

  GrObj::setUnRenderMode(GrObj::CLEAR_BOUNDING_BOX);
  GrObj::setScalingMode(GrObj::MAINTAIN_ASPECT_SCALING);
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

Rect<double> Gabor::grGetBoundingBox() const
{
DOTRACE("Gabor::grGetBoundingBox");

  Rect<double> bbox;
  bbox.left() = -0.5;
  bbox.right() = 0.5;
  bbox.bottom() = -0.5;
  bbox.top() = 0.5;

  return bbox;
}

void Gabor::grRender(GWT::Canvas&, DrawMode) const
{
DOTRACE("Gabor::grRender");
  double xsigma = sigma()*aspectRatio();
  double ysigma = sigma();
  double xmean = 0.0;
  double ymean = 0.0;

  static const double PI = acos(-1.0);

  //                      1            ( -(x-mean)^2 / (2*sigma)^2 )
  // gaussian(x) = ---------------- * e
  //               sigma*sqrt(2*pi)

  // For our purposes, we ignore the scaling factor out front, so that
  // the maximum value the function takes on is 1 (the scaling factor
  // is otherwise there to keep the total area under the curve to 1,
  // but this affects the maximum height).

  double res_step = 1.0/resolution();

  glPushAttrib(GL_POLYGON_BIT | GL_POINT_BIT);
  {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glPointSize(pointSize());

    for (int x_step = 0; x_step < resolution(); ++x_step)
      {
        double unrotated_x = x_step*res_step - 0.5;

        double x = unrotated_x;

        for (int y_step = 0; y_step < resolution(); ++y_step)
          {
            double unrotated_y = y_step*res_step - 0.5;

            double y = unrotated_y;

            if ( orientation() != 0 )
              {
                if ( orientation() == 90 )
                  {
                    x = -unrotated_y;
                    y = unrotated_x;
                  }
                else if ( orientation() == 180 )
                  {
                    x = -unrotated_x;
                    y = -unrotated_y;
                  }
                else if ( orientation() == 270 )
                  {
                    x = unrotated_y;
                    y= -unrotated_x;
                  }
                else
                  {
                    double r = sqrt(unrotated_x*unrotated_x +
                                    unrotated_y*unrotated_y);

                    double atan_y_x = atan2(unrotated_y, unrotated_x);

                    double new_theta = atan_y_x + orientation()*PI/180.0;

                    while (new_theta > PI)
                      {
                        new_theta -= 2*PI;
                      }
                    while (new_theta < -PI)
                      {
                        new_theta += 2*PI;
                      }

                    double unscaled_new_x = 1.0;
                    double unscaled_new_y = Util::abs(tan(new_theta));

                    double scale_factor =
                      sqrt(unscaled_new_x*unscaled_new_x +
                           unscaled_new_y*unscaled_new_y);

                    x = r * unscaled_new_x / scale_factor;
                    y = r * unscaled_new_y / scale_factor;

                    if (new_theta < -PI/2.0)
                      {
                        x = -x;
                        y = -y;
                      }
                    else if (new_theta < 0.0)
                      {
                        y = -y;
                      }
                    else if (new_theta < PI/2.0)
                      {
                        ; // nothing
                      }
                    else
                      {
                        x = -x;
                      }
                  }
              }

            double gauss_x = exp( -1.0*(x-xmean)*(x-xmean) /
                                  (4.0*xsigma*xsigma) );

            double sin_x = sin(2*PI*spatialFreq()*x + phase()*PI/180.0);

            double gauss_y = exp( -1.0*(y-ymean)*(y-ymean) /
                                  (4.0*ysigma*ysigma) );

            double gabor = 0.5*sin_x*gauss_x*gauss_y*contrast() + 0.5;

            if ( colorMode() == GRAYSCALE )
              {
                glColor4d(gabor, gabor, gabor, 1.0);
                glBegin(GL_POINTS);
                glVertex2d(unrotated_x, unrotated_y);
                glEnd();
              }
            else if ( colorMode() == COLOR_INDEX )
              {
                glIndexd(gabor*255.0);
                glBegin(GL_POINTS);
                glVertex2d(unrotated_x, unrotated_y);
                glEnd();
              }
            else if ( colorMode() == BW_DITHER_POINT )
              {
                if ( Util::randDoubleRange(0.0, 1.0) < gabor )
                  {
                    glBegin(GL_POINTS);
                    glVertex2d(unrotated_x, unrotated_y);
                    glEnd();
                  }
              }
            else if ( colorMode() == BW_DITHER_RECT )
              {
                if ( Util::randDoubleRange(0.0, 1.0) < gabor )
                  {
                    glRectd(unrotated_x, unrotated_y,
                            unrotated_x+res_step, unrotated_y+res_step);
                  }
              }
          }
      }
  }
  glPopAttrib();
}

static const char vcid_gabor_cc[] = "$Header$";
#endif // !GABOR_CC_DEFINED
