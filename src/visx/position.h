///////////////////////////////////////////////////////////////////////
//
// position.h
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Mar 10 21:33:14 1999
// written: Wed Nov 29 13:28:36 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef POSITION_H_DEFINED
#define POSITION_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(GXNODE_H_DEFINED)
#include "gx/gxnode.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(GXVEC_H_DEFINED)
#include "gx/gxvec.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(FIELDS_H_DEFINED)
#include "io/fields.h"
#endif

class PositionImpl;

///////////////////////////////////////////////////////////////////////
/**
 *
 * The Position class provides an interface for storing a particular
 * manipulation of the OpenGL modelview matrix. This manipulation is
 * decomposed into translation, scaling, and rotation.
 *
 **/
///////////////////////////////////////////////////////////////////////

class Position : public GxNode, public FieldContainer {
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
  GxVec3<double> translation;

  /// The scaling factors.
  GxVec3<double> scaling;

  /// The axis around which the rotation is performed.
  GxVec3<double> rotationAxis;

  /// The angle of rotation in degrees.
  TField<double> rotationAngle;

  static const FieldMap& classFields();

  /////////////
  // actions //
  /////////////

  /// Translate, scale, and rotate.
  virtual void draw(GWT::Canvas&) const;

  /** Redo most recent draw(). (This is useful if the behavior of
      draw() changes from one call to the next.) */
  virtual void undraw(GWT::Canvas&) const;

private:
  Position(const Position&);
  Position& operator=(const Position&);

  /// opaque pointer to implementation
  PositionImpl* const itsImpl;
};

const char vcid_position_h[] = "$Header$";
#endif // !POSITION_H_DEFINED
