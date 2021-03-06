/** @file gfx/gxtransform.h GxNode subclass for transforming the
    modelview matrix, to achieve translation, scaling, and/or
    rotation */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Wed Nov 20 15:15:24 2002
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [https://github.com/rjpcal/groovx]
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

#ifndef GROOVX_GFX_GXTRANSFORM_H_UTC20050626084024_DEFINED
#define GROOVX_GFX_GXTRANSFORM_H_UTC20050626084024_DEFINED

#include "gfx/gbvec.h"
#include "gfx/gxnode.h"

#include "io/fields.h"

class GxTransformImpl;

namespace geom
{
  class txform;
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
  virtual ~GxTransform() noexcept;

  virtual io::version_id class_version_id() const override;
  virtual void read_from(io::reader& reader) override;
  virtual void write_to(io::writer& writer) const override;


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
  const geom::txform& getTxform() const;

  /// Apply our transformation to the bbox.
  virtual void getBoundingCube(Gfx::Bbox& bbox) const override;

  /// Translate, scale, and rotate.
  virtual void draw(Gfx::Canvas&) const override;

private:
  GxTransform(const GxTransform&);
  GxTransform& operator=(const GxTransform&);

  /// opaque pointer to implementation
  GxTransformImpl* const rep;

  friend class GxTransformImpl;
};

#endif // !GROOVX_GFX_GXTRANSFORM_H_UTC20050626084024_DEFINED
