/** @file visx/morphyface.h highly-parameterized cartoon-style faces */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Wed Sep  8 15:37:45 1999
// commit: $Id$
// $HeadURL$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
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

#ifndef GROOVX_VISX_MORPHYFACE_H_UTC20050626084015_DEFINED
#define GROOVX_VISX_MORPHYFACE_H_UTC20050626084015_DEFINED

#include "gfx/gxshapekit.h"

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c MorphyFace is a subclass of \c GxShapeKit for drawing
 * highly-parameterized cartoon-style faces.
 *
 **/
///////////////////////////////////////////////////////////////////////

class MorphyFace : public GxShapeKit
{
  //////////////
  // creators //
  //////////////

protected:
  /// Default constructor.
  MorphyFace();

public:
  /// Default creator.
  static MorphyFace* make();

  /// Virtual destructor.
  virtual ~MorphyFace () throw();

  virtual io::version_id class_version_id() const;
  virtual void read_from(io::reader& reader);
  virtual void write_to(io::writer& writer) const;

  ////////////////
  // properties //
  ////////////////

  virtual int category() const { return itsMfaceCategory; }
  virtual void setCategory(int val) { itsMfaceCategory = val; }

private:
  /** The category of the face. The semantics of \a category are
      defined by the client. */
  int itsMfaceCategory;

  double itsFaceWidth;
    ///< The width of the face.
  double itsTopWidth;
    ///< The width of the top of the face, as a fraction of \a itsFaceWidth.
  double itsBottomWidth;
    ///< The width of the bottom of the face, as a fraction of \a itsFaceWidth.
  double itsTopHeight;
    ///< The height of the top of the face.
  double itsBottomHeight;
    ///< The height of the bottom of the face.

  double itsHairWidth;
    ///< The width of the hair.
  int itsHairStyle;
    ///< The style of the hair.

  double itsEyeYpos;
    ///< The y-position of the eyes, relative to the face midline.
  double itsEyeDistance;
    ///< The distance between the centers of the eyes.
  double itsEyeHeight;
    ///< The height of the eyes themselves.
  double itsEyeAspectRatio;
    ///< The aspect ratio of the eyes (width over height).

  double itsPupilXpos;
    ///< The x-position of the pupils, relative to the center of the eye.
  double itsPupilYpos;
    ///< The y-position of the pupils, relative to the center of the eye.
  double itsPupilSize;
    ///< The diameter of the pupils, relative to the height of the eye.
  double itsPupilDilation;
    ///< The dilation of the pupil, on a scale of [0,1).

  double itsEyebrowXpos;
    ///< The x-position of the eyebrows, relative to the eye.
  double itsEyebrowYpos;
    ///< The y-position of the eyebrows, relative to the top of the eye.
  double itsEyebrowCurvature;
    ///< The degree of eyebrow curvature, relative to that of the eye.
  double itsEyebrowAngle;
    ///< The angle of image-plane rotation of the eyebrow.
  double itsEyebrowThickness;
    ///< The thickness of the eyebrow, relative to the default thickness.

  double itsNoseXpos;
    ///< The x-position of the nose, relative to the face midline.
  double itsNoseYpos;
    ///< The y-position of the nose, relative to the face midline.
  double itsNoseLength;
    ///< The length of the nose.
  double itsNoseWidth;
    ///< The width of the nose.

  double itsMouthXpos;
    ///< The x-position of the mouth, relative to the face midline.
  double itsMouthYpos;
    ///< The y-position of the mouth, relative to the face midline.
  double itsMouthWidth;
    ///< The width of the mouth.
  double itsMouthCurvature;
    ///< The degree of curvature of the mouth.

  double itsStrokeWidth;
    ///< Base stroke width.
  bool itsLineJoin;
    ///< Whether or not to do fancy line-joining.

public:
  static const FieldMap& classFields();

protected:
  virtual void grGetBoundingBox(Gfx::Bbox& bbox) const;

  virtual void grRender(Gfx::Canvas& canvas) const;

private:
  MorphyFace(const MorphyFace&);            // copy constructor not to be used
  MorphyFace& operator=(const MorphyFace&); // assignment operator not to be used
};

static const char vcid_groovx_visx_morphyface_h_utc20050626084015[] = "$Id$ $HeadURL$";
#endif // !GROOVX_VISX_MORPHYFACE_H_UTC20050626084015_DEFINED
