///////////////////////////////////////////////////////////////////////
//
// position.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Mar 10 21:33:14 1999
// written: Wed Sep 25 19:02:59 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef POSITION_H_DEFINED
#define POSITION_H_DEFINED

#include "gfx/gbvec.h"
#include "gfx/gxnode.h"

#include "io/fields.h"

class PositionImpl;

namespace Gfx
{
  class Txform;
}

///////////////////////////////////////////////////////////////////////
/**
 *
 * The Position class provides an interface for storing a particular
 * manipulation of the OpenGL modelview matrix. This manipulation is
 * decomposed into translation, scaling, and rotation.
 *
 **/
///////////////////////////////////////////////////////////////////////

class Position : public GxNode, public FieldContainer
{
  //////////////
  // creators //
  //////////////
protected:
  /// Default constructor.
  Position();

public:
  /// Default creator.
  static Position* make();

  /// Virtual destructor.
  virtual ~Position();

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
  virtual void getBoundingCube(Gfx::Box<double>& cube,
                               Gfx::Canvas& canvas) const;

  /// Translate, scale, and rotate.
  virtual void draw(Gfx::Canvas&) const;

private:
  Position(const Position&);
  Position& operator=(const Position&);

  void onChange();

  /// opaque pointer to implementation
  PositionImpl* const rep;

  friend class PositionImpl;
};

const char vcid_position_h[] = "$Header$";
#endif // !POSITION_H_DEFINED
