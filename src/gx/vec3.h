///////////////////////////////////////////////////////////////////////
//
// vec3.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Nov 28 18:27:19 2000
// written: Sun Aug 26 08:35:12 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef VEC3_H_DEFINED
#define VEC3_H_DEFINED

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

  Vec3 operator+(const Vec3<V>& rhs) const
    { return Vec3<V>(x() + rhs.x(), y() + rhs.y(), z()+rhs.z()); }

  Vec3 operator-(const Vec3<V>& rhs) const
    { return Vec3<V>(x() - rhs.x(), y() - rhs.y(), z()-rhs.z()); }
};

static const char vcid_vec3_h[] = "$Header$";
#endif // !VEC3.H_DEFINED
