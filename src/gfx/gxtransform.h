///////////////////////////////////////////////////////////////////////
//
// gxtransform.h
//
// Copyright (c) 2002-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Wed Nov 20 15:15:24 2002
// written: Wed Mar 19 12:45:57 2003
// $Id$
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
  //////////////
  // creators //
  //////////////
protected:
  /// Default constructor.
  GxTransform();

public:
  /// Default creator.
  static GxTransform* make();

  /// Virtual destructor.
  virtual ~GxTransform();

  virtual IO::VersionId serialVersionId() const;
  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;


  ////////////
  // fields //
  ////////////

  /// The translation vector.
  GbVec3<double> translation;

  /// The scaling factors.
  GbVec3<double> scaling;

  /// The axis around which the rotation is performed.
  GbVec3<double> rotationAxis;

protected:
  /// The angle of rotation in degrees.
  double itsRotationAngle;

  int itsMtxMode;

  int itsJackSize;

public:
  static const FieldMap& classFields();

  ///////////////
  // accessors //
  ///////////////

  const Gfx::Txform& getTxform() const;

  /////////////
  // actions //
  /////////////

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
