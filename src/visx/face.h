///////////////////////////////////////////////////////////////////////
//
// face.h
//
// Copyright (c) 1998-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Tue Dec  1 08:00:00 1998
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

#ifndef FACE_H_DEFINED
#define FACE_H_DEFINED

#include "gfx/gxshapekit.h"

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c Face is a \c GxShapeKit subclass designed for rendering Brunswik
 * faces. The faces are parameterized by nose length, mouth height, eye
 * height, and eye separation.
 *
 **/
///////////////////////////////////////////////////////////////////////

class Face : public GxShapeKit
{
protected:
  /// Construct with initial values for the Brunswik face parameters.
  Face (double eh=0.6, double es=0.4, double nl=0.4, double mh=-0.8, int categ=0);

public:
  //////////////
  // creators //
  //////////////

  /// Default creator.
  static Face* make();

  /// Virtual destructor.
  virtual ~Face ();

  virtual IO::VersionId serialVersionId() const;
  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

  ////////////////
  // properties //
  ////////////////

  /// Get Face's fields.
  static const FieldMap& classFields();

  ///////////////
  // accessors //
  ///////////////

  virtual int category() const;

protected:
  /// Returns an array of Bezier control points for face outline.
  virtual const double* getCtrlPnts() const;

  /// Returns the aspect ratio of eye outline.
  virtual double getEyeAspect() const;

  /// Returns the amount of vertical offset applied to all features.
  virtual double getVertOffset() const;

  //////////////////
  // manipulators //
  //////////////////

public:
  virtual void setCategory(int val);

protected:
  virtual void grGetBoundingBox(Gfx::Bbox& bbox) const;

  /** Reimplements the \c GxShapeKit pure virtual function. It renders a
      face with the appropriate parameters. */
  virtual void grRender(Gfx::Canvas& canvas) const;

private:
  /** The category of the face. The semantics of \a category are
      defined by the client. */
  int itsFaceCategory;

  /// The height of the eyes above the vertical midline of the face.
  double itsEyeHeight;

  /// The distance between the centers of the eyes.
  double itsEyeDistance;

  /// The length of the nose.
  double itsNoseLength;

  /// The (negative) height of the mouth below the vertical midline of the face.
  double itsMouthHeight;

  /// Each bit can cause one of the face parts to be hidden when it is drawn.
  int itsPartsMask;

  /// Whether the face is drawn as filled, or just as outlines (default false).
  bool isItFilled;

  Face(const Face&);
  Face& operator=(const Face&);
};

static const char vcid_face_h[] = "$Header$";
#endif // !FACE_H_DEFINED
