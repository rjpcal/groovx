///////////////////////////////////////////////////////////////////////
//
// morphyface.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Sep  8 15:37:45 1999
// written: Wed Nov 20 16:10:45 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef MORPHYFACE_H_DEFINED
#define MORPHYFACE_H_DEFINED

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
  virtual ~MorphyFace ();

  virtual IO::VersionId serialVersionId() const;
  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

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

public:
  static const FieldMap& classFields();

protected:
  virtual void grGetBoundingBox(Gfx::Bbox& bbox) const;

  virtual void grRender(Gfx::Canvas& canvas) const;

private:
  MorphyFace(const MorphyFace&);            // copy constructor not to be used
  MorphyFace& operator=(const MorphyFace&); // assignment operator not to be used
};

static const char vcid_morphyface_h[] = "$Header$";
#endif // !MORPHYFACE_H_DEFINED
