///////////////////////////////////////////////////////////////////////
//
// point.cc
//
// Copyright (c) 2001-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Thu Aug  9 14:38:13 2001
// written: Mon May 12 12:06:52 2003
// $Id$
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
    return degrees * M_PI/180.0;
  }

  double rad2deg(double radians)
  {
    return radians * 180.0/M_PI;
  }

  void normRad(double& radians)
  {
    while (radians > M_PI) { radians -= 2*M_PI; }
    while (radians <= -M_PI) { radians += 2*M_PI; }
  }

  void normDeg(double& degrees)
  {
    while (degrees > 180.0) { degrees -= 360.0; }
    while (degrees <= -180.0) { degrees += 360.0; }
  }

  const char X_POSITIVE = 1;
  const char Y_POSITIVE = 2;

  const char QUAD_1 = X_POSITIVE | Y_POSITIVE;
  const char QUAD_2 = Y_POSITIVE;
  const char QUAD_3 = 0;
  const char QUAD_4 = X_POSITIVE;

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

  if ( !(quadrant & X_POSITIVE) ) { point.x() *= -1.0; }
  if ( !(quadrant & Y_POSITIVE) ) { point.y() *= -1.0; }
}

static const char vcid_point_cc[] = "$Header$";
#endif // !VEC2_CC_DEFINED
