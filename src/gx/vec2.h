///////////////////////////////////////////////////////////////////////
//
// point.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Jan 28 12:54:13 1999
// written: Thu Aug  9 15:27:50 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef POINT_H_DEFINED
#define POINT_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(CMATH_DEFINED)
#include <cmath>
#define CMATH_DEFINED
#endif

template <class V> class Point;

namespace PointAlgo
{
  const double PI = 3.14159265359;

  double deg2rad(double degrees);
  double rad2deg(double radians);

  void normRad(double& radians);
  void normDeg(double& degrees);

  double thetaDeg(double y, double x);

  void setPolarPoint(Point<double>& point,
                     double length, double degrees);
}

template<class V>
class Point {
public:
  Point(V x=0, V y=0) : xx(x), yy(y) {}

  template <class U>
  Point& operator=(const Point<U>& other)
    { xx = other.x(); yy = other.y(); return *this; }

  V& x() { return xx; }
  V& y() { return yy; }

  const V& x() const { return xx; }
  const V& y() const { return yy; }

  Point abs() const
    { return Point(xx > 0 ? xx : -xx, yy > 0 ? yy : -yy); }

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
    Point<double> dpoint;
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

    setThetaDeg(thetaDeg() + degrees);
  }

  //
  // Point-scalar math
  //

  template <class U>
  void scaleBy(const U& factor) { xx *= factor; yy *= factor; }

  Point operator*(const V& factor) const
    { return Point<V>(xx * factor, yy * factor); }

  Point operator/(const V& factor) const
    { return Point<V>(xx / factor, yy / factor); }

  template <class U>
  Point& operator*=(const U& factor) { scaleBy(factor); return *this; }

  template <class U>
  Point& operator/=(const U& factor) { scaleBy(V(1)/factor); return *this; }


  //
  // Point-Point math
  //

  Point operator+(const Point<V>& rhs) const
    { return Point<V>(xx + rhs.x(), yy + rhs.y()); }

  Point operator-(const Point<V>& rhs) const
    { return Point<V>(xx - rhs.x(), yy - rhs.y()); }


  template <class U>
  Point operator*(const Point<U>& rhs) const
    { return Point(V(x() * rhs.x()), V(y() * rhs.y())); }

  template <class U>
  Point operator/(const Point<U>& rhs) const
    { return Point(V(x() / rhs.x()), V(y() / rhs.y())); }


  template <class U>
  Point& operator+=(const Point<U>& rhs)
    { xx += V(rhs.x()); yy += V(rhs.y()); return *this; }

  template <class U>
  Point& operator-=(const Point<U>& rhs)
    { xx -= V(rhs.x()); yy -= V(rhs.y()); return *this; }


  template <class U>
  Point& operator*=(const Point<U>& factor)
    { xx *= factor.x(); yy *= factor.y(); return *this; }

  template <class U>
  Point& operator/=(const Point<U>& factor)
    { xx /= factor.x(); yy /= factor.y(); return *this; }


private:
  V xx;
  V yy;
};

static const char vcid_point_h[] = "$Header$";
#endif // !POINT_H_DEFINED
