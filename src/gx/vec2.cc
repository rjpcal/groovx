///////////////////////////////////////////////////////////////////////
//
// point.cc
//
// Copyright (c) 2001-2003 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Aug  9 14:38:13 2001
// written: Mon Jan 13 11:04:47 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef VEC2_CC_DEFINED
#define VEC2_CC_DEFINED

#include "gx/vec2.h"

#include "util/algo.h"

#include <cmath>

#include "util/trace.h"

namespace Gfx { namespace PointAlgo
{
  double deg2rad(double degrees)
  {
    return degrees * PI/180.0;
  }

  double rad2deg(double radians)
  {
    return radians * 180.0/PI;
  }

  void normRad(double& radians)
  {
    while (radians > PI) { radians -= 2*PI; }
    while (radians <= -PI) { radians += 2*PI; }
  }

  void normDeg(double& degrees)
  {
    while (degrees > 180.0) { degrees -= 360.0; }
    while (degrees <= -180.0) { degrees += 360.0; }
  }

  const char X_POS = 1;
  const char Y_POS = 2;

  const char QUAD_1 = X_POS | Y_POS;
  const char QUAD_2 = Y_POS;
  const char QUAD_3 = 0;
  const char QUAD_4 = X_POS;

  char quadrantDeg(double degrees)
  {
    if (degrees <= -180.0)
      {
        return quadrantDeg(degrees+360.0);
      }
    else if (degrees <= -90.0)
      {
        return QUAD_3;
      }
    else if (degrees <= 0.0)
      {
        return QUAD_4;
      }
    else if (degrees <= 90.0)
      {
        return QUAD_1;
      }
    else if (degrees <= 180.0)
      {
        return QUAD_2;
      }

    return quadrantDeg(degrees-360.0);
  }
}}

double Gfx::PointAlgo::thetaDeg(double y, double x)
{
  return rad2deg(atan2(y,x));
}

void Gfx::PointAlgo::setPolarPoint(Gfx::Vec2<double>& point,
                                   double length, double degrees)
{
DOTRACE("Gfx::PointAlgo::setPolarPoint");
  normDeg(degrees);

  if (degrees == 0.0) { point.set(length, 0.0); return; }
  if (degrees == 90.0) { point.set(0.0, length); return; }
  if (degrees == 180.0) { point.set(-length, 0.0); return; }
  if (degrees == -90.0) { point.set(0.0, -length); return; }

  point.x() = 1.0;
  point.y() = Util::abs(tan(deg2rad(degrees)));

  point.setLength(length);

  char quadrant = quadrantDeg(degrees);

  if ( !(quadrant & X_POS) ) { point.x() *= -1.0; }
  if ( !(quadrant & Y_POS) ) { point.y() *= -1.0; }
}

double Gfx::PointAlgo::lengthHelper(double x, double y, double z)
{
  return sqrt(x*x + y*y + z*z);
}

static const char vcid_point_cc[] = "$Header$";
#endif // !VEC2_CC_DEFINED
