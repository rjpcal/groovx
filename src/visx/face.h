///////////////////////////////////////////////////////////////////////
//
// face.h
// Rob Peters 
// created: Dec-98
// written: Thu Nov  2 14:56:39 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FACE_H_DEFINED
#define FACE_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(GROBJ_H_DEFINED)
#include "grobj.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(PROPERTY_H_DEFINED)
#include "io/property.h"
#endif

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c Face is a \c GrObj subclass designed for rendering Brunswik
 * faces. The faces are parameterized by nose length, mouth height,
 * eye height, and eye separation.
 *
 **/
///////////////////////////////////////////////////////////////////////

class Face : public GrObj, public PropFriend<Face> {
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

  /// Info about a \c Face property.
  typedef PropertyInfo<Face> PInfo;

  /// Return the number of \c Face properties.
  static unsigned int numPropertyInfos();

  /// Return info on the i'th \c Face property.
  static const PInfo& getPropertyInfo(unsigned int i);

  /** The category of the face. The semantics of \a category are
      defined by the client. */
  CTProperty<Face, int> faceCategory;

  /// The height of the eyes above the vertical midline of the face.
  CTProperty<Face, double> eyeHeight;

  /// The distance between the centers of the eyes.
  CTProperty<Face, double> eyeDistance;

  /// The length of the nose.
  CTProperty<Face, double> noseLength;

  /// The (negative) height of the mouth below the vertical midline of the face.
  CTProperty<Face, double> mouthHeight;

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

  protected: virtual void grGetBoundingBox(Rect<double>& bounding_box,
														 int& border_pixels) const;

  protected: virtual bool grHasBoundingBox() const;

  /** Reimplements the \c GrObj pure virtual function. It renders a
		face with the appropriate parameters. */
  protected: virtual void grRender(GWT::Canvas& canvas) const; 

private:
  Face(const Face&);
  Face& operator=(const Face&);

  /// Check all invariants and return true if everything is OK.
  bool check() const;
};

static const char vcid_face_h[] = "$Header$";
#endif // !FACE_H_DEFINED
