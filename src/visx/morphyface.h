///////////////////////////////////////////////////////////////////////
//
// morphyface.h
// Rob Peters 
// created: Wed Sep  8 15:37:45 1999
// written: Thu Mar 30 09:50:01 2000
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
public:
  //////////////
  // creators //
  //////////////

  /// Default constructor.
  MorphyFace();

  /// Construct from an \c istream using \c deserialize().
  MorphyFace(istream &is, IO::IOFlag flag);

  /// Virtual destructor.
  virtual ~MorphyFace ();

  virtual void serialize(ostream &os, IO::IOFlag flag) const;
  virtual void deserialize(istream &is, IO::IOFlag flag);
  virtual int charCount() const;

  virtual unsigned long serialVersionId() const;
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
  CTProperty<MorphyFace, int> category;

  virtual int getCategory() const { return category.getNative(); }
  virtual void setCategory(int val) { category.setNative(val); }
  
  CTProperty<MorphyFace, double> faceWidth;
    ///< The width of the face.
  CTProperty<MorphyFace, double> topWidth;
    ///< The width of the top of the face, as a fraction of \a faceWidth.
  CTProperty<MorphyFace, double> bottomWidth;
    ///< The width of the bottom of the face, as a fraction of \a faceWidth.
  CTProperty<MorphyFace, double> topHeight;
    ///< The height of the top of the face.
  CTProperty<MorphyFace, double> bottomHeight;
    ///< The height of the bottom of the face.

  CTProperty<MorphyFace, double> hairWidth;
    ///< The width of the hair.
  CTProperty<MorphyFace, int> hairStyle;
    ///< The style of the hair.

  CTProperty<MorphyFace, double> eyeYpos;
    ///< The y-position of the eyes, relative to the face midline.
  CTProperty<MorphyFace, double> eyeDistance;
    ///< The distance between the centers of the eyes.
  CTProperty<MorphyFace, double> eyeHeight;
    ///< The height of the eyes themselves.
  CTProperty<MorphyFace, double> eyeAspectRatio;
    ///< The aspect ratio of the eyes (width over height).

  CTProperty<MorphyFace, double> pupilXpos;
    ///< The x-position of the pupils, relative to the center of the eye.
  CTProperty<MorphyFace, double> pupilYpos;
    ///< The y-position of the pupils, relative to the center of the eye.
  CTProperty<MorphyFace, double> pupilSize;
    ///< The diameter of the pupils, relative to the height of the eye.
  CTBoundedProperty<MorphyFace, double, 0, 999, 1000> pupilDilation;
    ///< The dilation of the pupil, on a scale of [0,1).

  CTProperty<MorphyFace, double> eyebrowXpos;
    ///< The x-position of the eyebrows, relative to the eye.
  CTProperty<MorphyFace, double> eyebrowYpos;
    ///< The y-position of the eyebrows, relative to the top of the eye.
  CTProperty<MorphyFace, double> eyebrowCurvature;
    ///< The degree of eyebrow curvature, relative to that of the eye.
  CTProperty<MorphyFace, double> eyebrowAngle;
    ///< The angle of image-plane rotation of the eyebrow.
  CTProperty<MorphyFace, double> eyebrowThickness;
    ///< The thickness of the eyebrow, relative to the default thickness.

  CTProperty<MorphyFace, double> noseXpos;
    ///< The x-position of the nose, relative to the face midline.
  CTProperty<MorphyFace, double> noseYpos;
    ///< The y-position of the nose, relative to the face midline.
  CTProperty<MorphyFace, double> noseLength;
    ///< The length of the nose.
  CTProperty<MorphyFace, double> noseWidth;
    ///< The width of the nose.

  CTProperty<MorphyFace, double> mouthXpos;
    ///< The x-position of the mouth, relative to the face midline.
  CTProperty<MorphyFace, double> mouthYpos;
    ///< The y-position of the mouth, relative to the face midline.
  CTProperty<MorphyFace, double> mouthWidth;
    ///< The width of the mouth.
  CTProperty<MorphyFace, double> mouthCurvature;
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
