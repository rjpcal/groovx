///////////////////////////////////////////////////////////////////////
//
// gxtransform.h
//
// Copyright (c) 2002-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Wed Nov 20 15:15:24 2002
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

#ifndef GXTRANSFORM_H_DEFINED
#define GXTRANSFORM_H_DEFINED

#include "gfx/gbvec.h"
#include "gfx/gxnode.h"

#include "io/fields.h"

class GxTransformImpl;

namespace Gfx
{
  class Txform;
}

///////////////////////////////////////////////////////////////////////
/**
 *
 * The GxTransform class provides an interface for storing a particular
 * manipulation of the OpenGL modelview matrix. This manipulation is
 * decomposed into translation, scaling, and rotation.
 *
 **/
///////////////////////////////////////////////////////////////////////

class GxTransform : public GxNode, public FieldContainer
{
protected:
  /// Default constructor.
  GxTransform();

public:
  /// Default creator.
  static GxTransform* make();

  /// Virtual destructor.
  virtual ~GxTransform() throw();

  virtual IO::VersionId serialVersionId() const;
  virtual void readFrom(IO::Reader& reader);
  virtual void writeTo(IO::Writer& writer) const;


  /// The translation vector.
  GbVec3<double> translation;

  /// The scaling factors.
  GbVec3<double> scaling;

  /// The axis around which the rotation is performed.
  GbVec3<double> rotationAxis;

protected:
  /// The angle of rotation in degrees.
  double itsRotationAngle;

  /// The mode for computing the matrix transformation.
  int itsMtxMode;

  /// The size of the "jack" drawn at the origin.
  int itsJackSize;

public:
  /// Get GxTransform's fields.
  static const FieldMap& classFields();

  /// Get the associated coordinate transform matrix.
  const Gfx::Txform& getTxform() const;

  /// Apply our transformation to the bbox.
  virtual void getBoundingCube(Gfx::Bbox& bbox) const;

  /// Translate, scale, and rotate.
  virtual void draw(Gfx::Canvas&) const;

private:
  GxTransform(const GxTransform&);
  GxTransform& operator=(const GxTransform&);

  /// opaque pointer to implementation
  GxTransformImpl* const rep;

  friend class GxTransformImpl;
};

static const char vcid_gxtransform_h[] = "$Header$";
#endif // !GXTRANSFORM_H_DEFINED
