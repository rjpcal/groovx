///////////////////////////////////////////////////////////////////////
//
// point.cc
//
// Copyright (c) 2001-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Thu Aug  9 14:38:13 2001
// written: Mon May 12 18:49:30 2003
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

namespace Geom
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
}

static const char vcid_point_cc[] = "$Header$";
#endif // !VEC2_CC_DEFINED
