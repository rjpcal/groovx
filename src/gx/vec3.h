///////////////////////////////////////////////////////////////////////
//
// vec3.h
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Nov 28 18:27:19 2000
// written: Tue Nov 28 18:33:42 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef VEC3_H_DEFINED
#define VEC3_H_DEFINED

template <class T>
class Vec3 {
private:
  T itsData[3];

public:
  Vec3() { itsData[0] = itsData[1] = itsData[2] = T(); }
  Vec3(T x_, T y_, T z_) { x() = x_; y() = y_; z() = z_; }

        T& x()       { return itsData[0]; }
  const T& x() const { return itsData[0]; }

        T& y()       { return itsData[1]; }
  const T& y() const { return itsData[1]; }

        T& z()       { return itsData[2]; }
  const T& z() const { return itsData[2]; }

        T* data()       { return &itsData[0]; }
  const T* data() const { return &itsData[0]; }

  template <class V>
  Vec3 operator+(const Vec3<V>& rhs) const
	 { return Vec3<V>(x() + rhs.x(), y() + rhs.y(), z()+rhs.z()); }

  template <class V>
  Vec3 operator-(const Vec3<V>& rhs) const
	 { return Vec3<V>(x() - rhs.x(), y() - rhs.y(), z()-rhs.z()); }
};

static const char vcid_vec3_h[] = "$Header$";
#endif // !VEC3.H_DEFINED
