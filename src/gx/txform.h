///////////////////////////////////////////////////////////////////////
//
// txform.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Jun 21 13:57:32 2002
// written: Fri Jun 21 14:03:36 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TXFORM_H_DEFINED
#define TXFORM_H_DEFINED

namespace Gfx
{

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

  const double* colMajorData() const { return data; }

private:
  double data[16];
};

} // end namespace Gfx

static const char vcid_txform_h[] = "$Header$";
#endif // !TXFORM_H_DEFINED
