///////////////////////////////////////////////////////////////////////
//
// txform.h
//
// Copyright (c) 2002-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Fri Jun 21 13:57:32 2002
// commit: $Id$
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
  void rotate(const Vec3<double>& rotationAxis, double rotationAngle);
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
