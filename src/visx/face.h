///////////////////////////////////////////////////////////////////////
//
// face.h
// Rob Peters 
// created: Dec-98
// written: Mon Dec  6 21:32:34 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FACE_H_DEFINED
#define FACE_H_DEFINED

#ifndef VECTOR_DEFINED
#include <vector>
#define VECTOR_DEFINED
#endif

#ifndef GROBJ_H_DEFINED
#include "grobj.h"
#endif

#ifndef PROPERTY_H_DEFINED
#include "property.h"
#endif

///////////////////////////////////////////////////////////////////////
/**
 *
 * The Face class is derived from GrObj. Face provides the
 * functionality needed to display Brunswick faces parameterized by
 * nose length, mouth height, eye height, and eye separation.
 *
 * @short Subclass of GrObj for drawing Brunswik faces.
 **/
///////////////////////////////////////////////////////////////////////

class Face : public GrObj, public PropFriend<Face> {
public:
  //////////////
  // creators //
  //////////////

  ///
  Face (double eh=0.6, double es=0.4, double nl=0.4, double mh=-0.8, int categ=0);

  ///
  Face (istream &is, IOFlag flag);

  ///
  virtual ~Face ();

  /**Write the face to a stream using the format:
	  
	  [Face] category eyeHgt eyeDist noseLen mouthHgt */
  virtual void serialize(ostream &os, IOFlag flag) const;

  /**Read the face from a stream using the format:
	  
	  [Face] category eyeHgt eyeDist noseLen mouthHgt */
  virtual void deserialize(istream &is, IOFlag flag);
  
  ///
  virtual int charCount() const;

  ///
  virtual void readFrom(Reader* reader);
  ///
  virtual void writeTo(Writer* writer) const;

  ////////////////
  // properties //
  ////////////////

  ///
  typedef PropertyInfo<Face> PInfo;
  ///
  static const vector<PInfo>& getPropertyInfos();

  ///
  CTProperty<Face, int> category;

  ///
  CTProperty<Face, double> eyeHeight;
  ///
  CTProperty<Face, double> eyeDistance;
  ///
  CTProperty<Face, double> noseLength;
  ///
  CTProperty<Face, double> mouthHeight;

  ///////////////
  // accessors //
  ///////////////

  ///
  virtual int getCategory() const { return category.getNative(); }

protected:
  /// Returns an array of Bezier control points for face outline
  virtual const double* getCtrlPnts() const;

  /// Returns the aspect ratio of eye outline.
  virtual double getEyeAspect() const;
  
  /// Returns the amount of vertical offset applied to all features.
  virtual double getVertOffset() const;

  //////////////////
  // manipulators //
  //////////////////

public:
  ///
  virtual void setCategory(int val) { category.setNative(val); }

  ///
  protected: virtual bool grGetBoundingBox(Rect<double>& bounding_box,
														 int& border_pixels) const;

  /** This overrides GrObj pure virtual function. It renders a face
		with the appropriate parameters. */
  protected: virtual void grRender(Canvas& canvas) const; 

private:
  /// Check all invariants and return true if everything is OK.
  bool check() const;

  /// copy constructor not to be used
  Face(const Face&);

  /// assignment operator not to be used
  Face& operator=(const Face&);

  ///
  void makeIoList(vector<IO *>& vec);
  ///
  void makeIoList(vector<const IO *>& vec) const;
};

static const char vcid_face_h[] = "$Header$";
#endif // !FACE_H_DEFINED
