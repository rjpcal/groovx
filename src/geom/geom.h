///////////////////////////////////////////////////////////////////////
//
// geom.h
//
// Copyright (c) 2002-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Mon May 12 11:16:03 2003
// commit: $Id$
// $HeadURL$
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

#ifndef GEOM_H_DEFINED
#define GEOM_H_DEFINED

#include <cmath>

namespace geom
{
  inline double deg2rad(double degrees)
  {
    return degrees * M_PI/180.0;
  }

  inline double rad2deg(double radians)
  {
    return radians * 180.0/M_PI;
  }

  inline double deg_n180_180(double degrees)
  {
    while (degrees > 180.0) { degrees -= 360.0; }
    while (degrees <= -180.0) { degrees += 360.0; }

    return degrees;
  }

  inline double rad_0_2pi(double angle)
  {
    while (angle < 0.0)     angle += 2*M_PI;
    while (angle >= 2*M_PI) angle -= 2*M_PI;

    return angle;
  }

  inline double rad_npi_pi(double angle)
  {
    while (angle < -M_PI) angle += 2*M_PI;
    while (angle >= M_PI) angle -= 2*M_PI;

    return angle;
  }

  inline double rad_0_pi(double angle)
  {
    while (angle < 0.0)   angle += M_PI;
    while (angle >= M_PI) angle -= M_PI;

    return angle;
  }
}

static const char vcid_geom_h[] = "$Id$ $URL$";
#endif // !GEOM_H_DEFINED
