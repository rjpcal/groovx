///////////////////////////////////////////////////////////////////////
//
// txform.h
//
// Copyright (c) 2002-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Fri Jun 21 13:57:32 2002
// written: Wed Mar 19 12:45:52 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TXFORM_H_DEFINED
#define TXFORM_H_DEFINED

namespace Gfx
{

template <class V> class Vec2;
template <class V> class Vec3;

/// Represents a 4x4 transformation matrix in homogenous coordinates.
class Txform
{
public:
  /// Build an identity transformation matrix.
  Txform();

  Txform(const Vec3<double>& translation,
         const Vec3<double>& scaling,
         const Vec3<double>& rotationAxis,
         double rotationAngle);

  void translate(const Vec3<double>& t);
  void scale(const Vec3<double>& s);
  void transform(const Gfx::Txform& other);

  Vec2<double> applyTo(const Vec2<double>& input) const;
  Vec3<double> applyTo(const Vec3<double>& input) const;

  const double* colMajorData() const { return data; }

  void debugDump() const;

private:
  double data[16];
};

} // end namespace Gfx

static const char vcid_txform_h[] = "$Header$";
#endif // !TXFORM_H_DEFINED
