///////////////////////////////////////////////////////////////////////
//
// vec3.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Nov 28 18:27:19 2000
// written: Fri Jan 18 16:06:57 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef VEC3_H_DEFINED
#define VEC3_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(VEC2_H_DEFINED)
#include "gx/vec2.h"
#endif

namespace Gfx
{
  template <class V> class Vec3;
}

template <class V>
class Gfx::Vec3
{
private:
  V itsData[3];

public:
  Vec3() { itsData[0] = itsData[1] = itsData[2] = V(); }

  Vec3(V x_, V y_, V z_) { set(x_, y_, z_); }

  Vec3(const V* arr) { set(arr[0], arr[1], arr[2]); }

  template <class U>
  Vec3(const Vec3<U>& other)
    { set(other.x(), other.y(), other.z()); }

  static Vec3<V> unitX() { return Vec3<V>(1.0, 0.0, 0.0); }
  static Vec3<V> unitY() { return Vec3<V>(0.0, 1.0, 0.0); }
  static Vec3<V> unitZ() { return Vec3<V>(0.0, 0.0, 1.0); }

        V& x()       { return itsData[0]; }
  const V& x() const { return itsData[0]; }

        V& y()       { return itsData[1]; }
  const V& y() const { return itsData[1]; }

        V& z()       { return itsData[2]; }
  const V& z() const { return itsData[2]; }

  void get(V& x_, V& y_, V& z_) const { x_ = x(); y_ = y(); z_ = z(); }
  void set(V x_, V y_, V z_)          { x() = x_; y() = y_; z() = z_; }

        V* data()       { return &itsData[0]; }
  const V* data() const { return &itsData[0]; }

  Gfx::Vec2<V> vec2() const { return Gfx::Vec2<V>(x(), y()); }

  //
  // Vec3-scalar math
  //

  template <class U>
  void scaleBy(const U& factor)
    { x() *= factor; y() *= factor; z() *= factor; }

  template <class U>
  Vec3& operator*=(const U& factor) { scaleBy(factor); return *this; }

  template <class U>
  Vec3& operator/=(const U& factor) { scaleBy(1.0/factor); return *this; }

  Vec3 operator*(const V& factor) const
    { Vec3<V> copy(*this); copy *= factor; return copy; }

  Vec3 operator/(const V& factor) const
    { Vec3<V> copy(*this); copy /= factor; return copy; }

  //
  // Vec3-Vec3 math
  //

  Vec3 operator+(const Vec3<V>& rhs) const
    { return Vec3<V>(x() + rhs.x(), y() + rhs.y(), z() + rhs.z()); }

  Vec3 operator-(const Vec3<V>& rhs) const
    { return Vec3<V>(x() - rhs.x(), y() - rhs.y(), z() - rhs.z()); }

  Vec3 operator*(const Vec3<V>& rhs) const
    { return Vec3<V>(x() * rhs.x(), y() * rhs.y(), z() * rhs.z()); }

  Vec3 operator/(const Vec3<V>& rhs) const
    { return Vec3<V>(x() / rhs.x(), y() / rhs.y(), z() / rhs.z()); }
};

static const char vcid_vec3_h[] = "$Header$";
#endif // !VEC3.H_DEFINED
