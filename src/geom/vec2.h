///////////////////////////////////////////////////////////////////////
//
// point.h
//
// Copyright (c) 1999-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Thu Jan 28 12:54:13 1999
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

#ifndef VEC2_H_DEFINED
#define VEC2_H_DEFINED

#include "geom/geom.h"

#include <cmath>

namespace Gfx
{
  template <class V> class Vec2;

/// Gfx::Vec2 is a 2-D vector class for representing 2-D points or distances.
template<class V>
class Vec2
{
public:
  Vec2(V x=0, V y=0) : xx(x), yy(y) {}

  template <class U>
  explicit Vec2(const Vec2<U>& other) : xx(V(other.x())), yy(V(other.y())) {}

  template <class U>
  Vec2& operator=(const Vec2<U>& other)
    { xx = other.x(); yy = other.y(); return *this; }

  V& x() { return xx; }
  V& y() { return yy; }

  const V& x() const { return xx; }
  const V& y() const { return yy; }

  Vec2 abs() const
    { return Vec2(xx > 0 ? xx : -xx, yy > 0 ? yy : -yy); }

  void set(V x, V y) { xx = x; yy = y; }

  bool operator==(const Vec2<V>& b)
    { return x() == b.x() && y() == b.y(); }

  //
  // Polar coordinates
  //

  double length() const { return sqrt(xx*xx + yy*yy); }

  void setLength(double len)
  {
    const double r = length();
    if (r != 0.0)
      scaleBy(len / r);
  }

  void setPolarRad(double r, double theta)
  {
    xx = r * cos(theta);
    yy = r * sin(theta);
  }

  double thetaDeg() const
  {
    return Geom::rad2deg(atan2(yy, xx));
  }

  void setThetaDeg(double degrees)
  {
    setPolarRad(length(), Geom::deg2rad(degrees));
  }

  void rotateDeg(double degrees)
  {
    // FIXME should use a real sin(),cos() rotation matrix here?
    degrees = Geom::deg_n180_180(degrees);
    if (degrees == 0.0)
      {
        return;
      }
    else if (degrees == 90.0)
      {
        double old_x = xx;
        xx = -yy;
        yy = old_x;
      }
    else if (degrees == 180.0)
      {
        xx = -xx;
        yy = -yy;
      }
    else if (degrees == -90.0)
      {
        double old_x = xx;
        xx = yy;
        yy = -old_x;
      }
    else
      {
        setThetaDeg(thetaDeg() + degrees);
      }
  }

  /// Result in radians.
  double angleTo(const Vec2<V>& b) const
  {
    return rad_0_2pi(atan2(b.y() - y(), b.x() - x()));
  }

  double distanceTo(const Vec2<V>& b) const
  {
    const double dx = x() - b.x();
    const double dy = y() - b.y();
    return sqrt(dx*dx + dy*dy);
  }

  //
  // Vec2-scalar math
  //

  template <class U>
  void scaleBy(const U& factor) { xx *= factor; yy *= factor; }

  Vec2 operator*(const V& factor) const
    { return Vec2<V>(xx * factor, yy * factor); }

  Vec2 operator/(const V& factor) const
    { return Vec2<V>(xx / factor, yy / factor); }

  template <class U>
  Vec2& operator*=(const U& factor) { scaleBy(factor); return *this; }

  template <class U>
  Vec2& operator/=(const U& factor) { scaleBy(V(1)/factor); return *this; }


  //
  // Vec2-Vec2 math
  //

  Vec2 operator+(const Vec2<V>& rhs) const
    { return Vec2<V>(xx + rhs.x(), yy + rhs.y()); }

  Vec2 operator-(const Vec2<V>& rhs) const
    { return Vec2<V>(xx - rhs.x(), yy - rhs.y()); }


  template <class U>
  Vec2 operator*(const Vec2<U>& rhs) const
    { return Vec2(V(x() * rhs.x()), V(y() * rhs.y())); }

  template <class U>
  Vec2 operator/(const Vec2<U>& rhs) const
    { return Vec2(V(x() / rhs.x()), V(y() / rhs.y())); }


  template <class U>
  Vec2& operator+=(const Vec2<U>& rhs)
    { xx += V(rhs.x()); yy += V(rhs.y()); return *this; }

  template <class U>
  Vec2& operator-=(const Vec2<U>& rhs)
    { xx -= V(rhs.x()); yy -= V(rhs.y()); return *this; }


  template <class U>
  Vec2& operator*=(const Vec2<U>& factor)
    { xx *= factor.x(); yy *= factor.y(); return *this; }

  template <class U>
  Vec2& operator/=(const Vec2<U>& factor)
    { xx /= factor.x(); yy /= factor.y(); return *this; }


private:
  V xx;
  V yy;
};

  typedef Vec2<int> Vec2i;
  typedef Vec2<float> Vec2f;
  typedef Vec2<double> Vec2d;

  template <class U>
  Vec2<U> normalTo(const Vec2<U>& a)
    { return Vec2<U>(-a.y(), a.x()); }

  template <class U>
  Vec2<U> makeUnitLength(const Vec2<U>& a)
    {
      const double r = a.length();
      if (r == 0.0) return a;
      return Vec2<U>(a.x()/r, a.y()/r);
    }

} // end namespace Gfx

static const char vcid_point_h[] = "$Header$";
#endif // !VEC2_H_DEFINED
