///////////////////////////////////////////////////////////////////////
//
// morphyface.h
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Sep  8 15:37:45 1999
// written: Tue Nov 14 11:33:40 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef MORPHYFACE_H_DEFINED
#define MORPHYFACE_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(GROBJ_H_DEFINED)
#include "grobj.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(FIELDS_H_DEFINED)
#include "io/fields.h"
#endif

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c MorphyFace is a subclass of \c GrObj for drawing
 * highly-parameterized cartoon-style faces.
 *
 **/
///////////////////////////////////////////////////////////////////////

class MorphyFace : public GrObj, public FieldContainer {
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

  /** The category of the face. The semantics of \a category are
      defined by the client. */
  TField<int> mfaceCategory;

  virtual int category() const { return mfaceCategory(); }
  virtual void setCategory(int val) { mfaceCategory.setNative(val); }

  TField<double> faceWidth;
    ///< The width of the face.
  TField<double> topWidth;
    ///< The width of the top of the face, as a fraction of \a faceWidth.
  TField<double> bottomWidth;
    ///< The width of the bottom of the face, as a fraction of \a faceWidth.
  TField<double> topHeight;
    ///< The height of the top of the face.
  TField<double> bottomHeight;
    ///< The height of the bottom of the face.

  TField<double> hairWidth;
    ///< The width of the hair.
  TField<int> hairStyle;
    ///< The style of the hair.

  TField<double> eyeYpos;
    ///< The y-position of the eyes, relative to the face midline.
  TField<double> eyeDistance;
    ///< The distance between the centers of the eyes.
  TField<double> eyeHeight;
    ///< The height of the eyes themselves.
  TField<double> eyeAspectRatio;
    ///< The aspect ratio of the eyes (width over height).

  TField<double> pupilXpos;
    ///< The x-position of the pupils, relative to the center of the eye.
  TField<double> pupilYpos;
    ///< The y-position of the pupils, relative to the center of the eye.
  TField<double> pupilSize;
    ///< The diameter of the pupils, relative to the height of the eye.
  TBoundedField<double, 0, 999, 1000> pupilDilation;
    ///< The dilation of the pupil, on a scale of [0,1).

  TField<double> eyebrowXpos;
    ///< The x-position of the eyebrows, relative to the eye.
  TField<double> eyebrowYpos;
    ///< The y-position of the eyebrows, relative to the top of the eye.
  TField<double> eyebrowCurvature;
    ///< The degree of eyebrow curvature, relative to that of the eye.
  TField<double> eyebrowAngle;
    ///< The angle of image-plane rotation of the eyebrow.
  TField<double> eyebrowThickness;
    ///< The thickness of the eyebrow, relative to the default thickness.

  TField<double> noseXpos;
    ///< The x-position of the nose, relative to the face midline.
  TField<double> noseYpos;
    ///< The y-position of the nose, relative to the face midline.
  TField<double> noseLength;
    ///< The length of the nose.
  TField<double> noseWidth;
    ///< The width of the nose.

  TField<double> mouthXpos;
    ///< The x-position of the mouth, relative to the face midline.
  TField<double> mouthYpos;
    ///< The y-position of the mouth, relative to the face midline.
  TField<double> mouthWidth;
    ///< The width of the mouth.
  TField<double> mouthCurvature;
    ///< The degree of curvature of the mouth.

  static const FieldMap& classFields();

protected:
  virtual void grGetBoundingBox(Rect<double>& bbox,
										  int& border_pixels) const;

  virtual bool grHasBoundingBox() const;

  virtual void grRender(GWT::Canvas& canvas) const; 

private:
  /// Check all invariants and return true if everything is OK.
  bool check() const;

  MorphyFace(const MorphyFace&);            // copy constructor not to be used
  MorphyFace& operator=(const MorphyFace&); // assignment operator not to be used
};

static const char vcid_morphyface_h[] = "$Header$";
#endif // !MORPHYFACE_H_DEFINED
