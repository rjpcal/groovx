///////////////////////////////////////////////////////////////////////
//
// gxcamera.cc
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Nov 21 15:22:25 2002
// written: Thu Nov 21 16:31:16 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXCAMERA_CC_DEFINED
#define GXCAMERA_CC_DEFINED

#include "gxcamera.h"

#include "gfx/canvas.h"

#include "util/error.h"

#include <cmath>

#include "util/debug.h"
#include "util/trace.h"

void GxPerspectiveCamera::setPerspective(double fovy, double zNear, double zFar)
{
DOTRACE("GxPerspectiveCamera::setPerspective");

  itsFovY = fovy;
  itsNearZ = zNear;
  itsFarZ = zFar;
}

void GxPerspectiveCamera::draw(Gfx::Canvas& canvas) const
{
DOTRACE("GxPerspectiveCamera::draw");

  canvas.viewport(0, 0, width(), height());

  canvas.perspective(itsFovY,
                     double(width()) / double(height()),
                     itsNearZ, itsFarZ);
}


void GxFixedRectCamera::draw(Gfx::Canvas& canvas) const
{
DOTRACE("GxFixedRectCamera::draw");

  canvas.viewport(0, 0, width(), height());
  canvas.orthographic(itsRect, -10.0, 10.0);
}

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

void GxFixedScaleCamera::setPixelsPerUnit(double s)
{
  if (s <= 0.0)
    throw Util::Error("invalid scaling factor");

  itsPixelsPerUnit = s;
}

void GxFixedScaleCamera::setUnitAngle(double deg_per_unit)
{
  if (deg_per_unit <= 0.0)
    throw Util::Error("unit angle must be positive");

  static const double deg_to_rad = 3.141593/180.0;

  // tan(deg_per_unit) == screen_inches_per_unit/viewing_dist;
  // screen_inches_per_unit == 1.0 * itsPixelsPerUnit / screepPpi;
  const double screen_inches_per_unit =
    tan(deg_per_unit*deg_to_rad) * itsViewingDistance;

  itsPixelsPerUnit = int(screen_inches_per_unit * itsScreenPixelsPerInch);
}

void GxFixedScaleCamera::setViewingDistIn(double inches)
{
  if (inches <= 0.0)
    throw Util::Error("viewing distance must be positive (duh)");

  // according to similar triangles,
  //   new_dist / old_dist == new_scale / old_scale;
  const double factor = inches / itsViewingDistance;
  itsPixelsPerUnit *= factor;
  itsViewingDistance = inches;
}

void GxFixedScaleCamera::draw(Gfx::Canvas& canvas) const
{
DOTRACE("GxFixedScaleCamera::draw");

  canvas.viewport(0, 0, width(), height());

  const double l = -1 * (width()  / 2.0) / itsPixelsPerUnit;
  const double r =      (width()  / 2.0) / itsPixelsPerUnit;
  const double b = -1 * (height() / 2.0) / itsPixelsPerUnit;
  const double t =      (height() / 2.0) / itsPixelsPerUnit;
  canvas.orthographic(Gfx::RectLTRB<double>(l, t, r, b), -10.0, 10.0);
}

static const char vcid_gxcamera_cc[] = "$Header$";
#endif // !GXCAMERA_CC_DEFINED
