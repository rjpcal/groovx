///////////////////////////////////////////////////////////////////////
//
// morphyface.h
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Sep  8 15:37:45 1999
// written: Sat Nov 11 10:21:38 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef MORPHYFACE_H_DEFINED
#define MORPHYFACE_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(GROBJ_H_DEFINED)
#include "grobj.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(PROPERTY_H_DEFINED)
#include "io/property.h"
#endif

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c MorphyFace is a subclass of \c GrObj for drawing
 * highly-parameterized cartoon-style faces.
 *
 **/
///////////////////////////////////////////////////////////////////////

class MorphyFace : public GrObj, public PropFriend<MorphyFace> {
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

  /// Info about a \c MorphyFace property.
  typedef PropertyInfo<MorphyFace> PInfo;

  /// Return the number of \c MorphyFace properties.
  static unsigned int numPropertyInfos();

  /// Return info on the i'th \c MorphyFace property.
  static const PInfo& getPropertyInfo(unsigned int i);

  /** The category of the face. The semantics of \a category are
      defined by the client. */
  TProperty<int> mfaceCategory;

  virtual int category() const { return mfaceCategory.getNative(); }
  virtual void setCategory(int val) { mfaceCategory.setNative(val); }

  TProperty<double> faceWidth;
    ///< The width of the face.
  TProperty<double> topWidth;
    ///< The width of the top of the face, as a fraction of \a faceWidth.
  TProperty<double> bottomWidth;
    ///< The width of the bottom of the face, as a fraction of \a faceWidth.
  TProperty<double> topHeight;
    ///< The height of the top of the face.
  TProperty<double> bottomHeight;
    ///< The height of the bottom of the face.

  TProperty<double> hairWidth;
    ///< The width of the hair.
  TProperty<int> hairStyle;
    ///< The style of the hair.

  TProperty<double> eyeYpos;
    ///< The y-position of the eyes, relative to the face midline.
  TProperty<double> eyeDistance;
    ///< The distance between the centers of the eyes.
  TProperty<double> eyeHeight;
    ///< The height of the eyes themselves.
  TProperty<double> eyeAspectRatio;
    ///< The aspect ratio of the eyes (width over height).

  TProperty<double> pupilXpos;
    ///< The x-position of the pupils, relative to the center of the eye.
  TProperty<double> pupilYpos;
    ///< The y-position of the pupils, relative to the center of the eye.
  TProperty<double> pupilSize;
    ///< The diameter of the pupils, relative to the height of the eye.
  TBoundedProperty<double, 0, 999, 1000> pupilDilation;
    ///< The dilation of the pupil, on a scale of [0,1).

  TProperty<double> eyebrowXpos;
    ///< The x-position of the eyebrows, relative to the eye.
  TProperty<double> eyebrowYpos;
    ///< The y-position of the eyebrows, relative to the top of the eye.
  TProperty<double> eyebrowCurvature;
    ///< The degree of eyebrow curvature, relative to that of the eye.
  TProperty<double> eyebrowAngle;
    ///< The angle of image-plane rotation of the eyebrow.
  TProperty<double> eyebrowThickness;
    ///< The thickness of the eyebrow, relative to the default thickness.

  TProperty<double> noseXpos;
    ///< The x-position of the nose, relative to the face midline.
  TProperty<double> noseYpos;
    ///< The y-position of the nose, relative to the face midline.
  TProperty<double> noseLength;
    ///< The length of the nose.
  TProperty<double> noseWidth;
    ///< The width of the nose.

  TProperty<double> mouthXpos;
    ///< The x-position of the mouth, relative to the face midline.
  TProperty<double> mouthYpos;
    ///< The y-position of the mouth, relative to the face midline.
  TProperty<double> mouthWidth;
    ///< The width of the mouth.
  TProperty<double> mouthCurvature;
    ///< The degree of curvature of the mouth.

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
