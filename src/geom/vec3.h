///////////////////////////////////////////////////////////////////////
//
// vec3.h
//
// Copyright (c) 2000-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Tue Nov 28 18:27:19 2000
// commit: $Id$
// $HeadURL$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
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

#ifndef GROOVX_GEOM_VEC3_H_UTC20050626084023_DEFINED
#define GROOVX_GEOM_VEC3_H_UTC20050626084023_DEFINED

#include "geom/vec2.h"

#include "rutz/debug.h"
GVX_DBG_REGISTER

namespace geom
{
  template <class V> class vec3;

  /// Gfx::vec3 is a 3-D vector class for representing 3-D points or distances.
  template <class V>
  class vec3
  {
  private:
    V m_d[3];

  public:
    vec3() { m_d[0] = m_d[1] = m_d[2] = V(); }

    vec3(V x_, V y_, V z_) { set(x_, y_, z_); }

    explicit vec3(const V* arr) { set(arr[0], arr[1], arr[2]); }

    template <class U>
    explicit vec3(const vec3<U>& other)
    { set(V(other.x()), V(other.y()), V(other.z())); }

    static vec3 zeros() { return vec3(V(0), V(0), V(0)); }
    static vec3 ones()  { return vec3(V(1), V(1), V(1)); }

    static vec3<V> unit_x() { return vec3<V>(1.0, 0.0, 0.0); }
    static vec3<V> unit_y() { return vec3<V>(0.0, 1.0, 0.0); }
    static vec3<V> unit_z() { return vec3<V>(0.0, 0.0, 1.0); }

    V& x()       { return m_d[0]; }
    const V& x() const { return m_d[0]; }

    V& y()       { return m_d[1]; }
    const V& y() const { return m_d[1]; }

    V& z()       { return m_d[2]; }
    const V& z() const { return m_d[2]; }

    void get(V& x_, V& y_, V& z_) const { x_ = x(); y_ = y(); z_ = z(); }
    void set(V x_, V y_, V z_)          { x() = x_; y() = y_; z() = z_; }

    V* data()       { return &m_d[0]; }
    const V* data() const { return &m_d[0]; }

    vec2<V> as_vec2() const { return vec2<V>(x(), y()); }

    double length() const
    { return sqrt(x()*x() + y()*y() + z()*z()); }

    //
    // vec3-scalar math
    //

    template <class U>
    void scale_by(const U& factor)
    { x() *= factor; y() *= factor; z() *= factor; }

    template <class U>
    vec3& operator*=(const U& factor) { scale_by(factor); return *this; }

    template <class U>
    vec3& operator/=(const U& factor) { scale_by(1.0/factor); return *this; }

    vec3 operator*(const V& factor) const
    { vec3<V> copy(*this); copy *= factor; return copy; }

    vec3 operator/(const V& factor) const
    { vec3<V> copy(*this); copy /= factor; return copy; }


    //
    // vec3-vec2 math (fill in z==0 for addition)
    //

    vec3 operator+(const vec2<V>& rhs) const
    { return vec3<V>(x() + rhs.x(), y() + rhs.y(), z()); }

    vec3 operator-(const vec2<V>& rhs) const
    { return vec3<V>(x() - rhs.x(), y() - rhs.y(), z()); }


    //
    // vec3-vec3 math
    //

    vec3 operator+(const vec3<V>& rhs) const
    { return vec3<V>(x() + rhs.x(), y() + rhs.y(), z() + rhs.z()); }

    vec3 operator-(const vec3<V>& rhs) const
    { return vec3<V>(x() - rhs.x(), y() - rhs.y(), z() - rhs.z()); }

    vec3 operator*(const vec3<V>& rhs) const
    { return vec3<V>(x() * rhs.x(), y() * rhs.y(), z() * rhs.z()); }

    vec3 operator/(const vec3<V>& rhs) const
    { return vec3<V>(x() / rhs.x(), y() / rhs.y(), z() / rhs.z()); }


    //
    // debugging
    //

    void debug_dump() const throw()
    {
      dbg_eval(0, x());
      dbg_eval(0, y());
      dbg_eval_nl(0, z());
    }
  };

  typedef vec3<int> vec3i;
  typedef vec3<float> vec3f;
  typedef vec3<double> vec3d;

} // end namespace geom

static const char vcid_groovx_geom_vec3_h_utc20050626084023[] = "$Id$ $HeadURL$";
#endif // !GROOVX_GEOM_VEC3_H_UTC20050626084023_DEFINED
