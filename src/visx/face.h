///////////////////////////////////////////////////////////////////////
//
// face.h
// Rob Peters 
// created: Dec-98
// written: Tue Sep 26 19:04:08 2000
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
public:
  //////////////
  // creators //
  //////////////

  /// Construct with initial values for the Brunswik face parameters.
  Face (double eh=0.6, double es=0.4, double nl=0.4, double mh=-0.8, int categ=0);
#ifdef LEGACY
  /// Construct from an \c STD_IO::istream using \c legacyDesrlz().
  Face (STD_IO::istream &is, IO::IOFlag flag);
#endif
  /// Virtual destructor.
  virtual ~Face ();

  /** Write the face to a stream using the format:
	  
	   <tt>[Face] category eyeHgt eyeDist noseLen mouthHgt</tt> */
  virtual void legacySrlz(IO::Writer* writer, STD_IO::ostream &os, IO::IOFlag flag) const;

  /** Read the face from a stream using the format:
	  
	   <tt>[Face] category eyeHgt eyeDist noseLen mouthHgt</tt> */
  virtual void legacyDesrlz(IO::Reader* reader, STD_IO::istream &is, IO::IOFlag flag);
  
  virtual int legacyCharCount() const;

  virtual unsigned long serialVersionId() const;
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
  CTProperty<Face, int> category;

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

  virtual int getCategory() const;

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
  /// Check all invariants and return true if everything is OK.
  bool check() const;

  /// copy constructor not to be used
  Face(const Face&);

  /// assignment operator not to be used
  Face& operator=(const Face&);
};

static const char vcid_face_h[] = "$Header$";
#endif // !FACE_H_DEFINED
