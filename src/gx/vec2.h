///////////////////////////////////////////////////////////////////////
//
// point.h
//
// Copyright (c) 1999-2003 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Jan 28 12:54:13 1999
// written: Mon Jan 13 11:04:47 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef VEC2_H_DEFINED
#define VEC2_H_DEFINED

#include <cmath>

namespace Gfx
{
  template <class V> class Vec2;

  namespace PointAlgo
  {
    const double PI = 3.14159265359;

    double deg2rad(double degrees);
    double rad2deg(double radians);

    void normRad(double& radians);
    void normDeg(double& degrees);

    double thetaDeg(double y, double x);

    void setPolarPoint(Vec2<double>& point,
                       double length, double degrees);

    double lengthHelper(double x, double y, double z = 0.0);
  }

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

  //
  // Polar coordinates
  //

  V length() const { return V(sqrt(xx*xx + yy*yy)); }

  void setLength(V len)
  {
    scaleBy(len / length());
  }

  double thetaDeg() const { return PointAlgo::thetaDeg(yy, xx); }

  void setThetaDeg(double degrees)
  {
    Vec2<double> dpoint;
    PointAlgo::setPolarPoint(dpoint, length(), degrees);
    operator=(dpoint);
  }

  void rotateDeg(double degrees)
  {
    PointAlgo::normDeg(degrees);
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

} // end namespace Gfx

static const char vcid_point_h[] = "$Header$";
#endif // !VEC2_H_DEFINED
