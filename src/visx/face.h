///////////////////////////////////////////////////////////////////////
//
// face.h
// Rob Peters 
// created: Dec-98
// written: Tue Oct 19 15:37:27 1999
// $Id$
//
// The Face class is derived from GrObj. Face provides the
// functionality needed to display Brunswick faces parameterized by
// nose length, mouth height, eye height, and eye separation.
//
///////////////////////////////////////////////////////////////////////

#ifndef FACE_H_DEFINED
#define FACE_H_DEFINED

#include <cmath>					  // for abs()

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
//
// Face class declaration
//
///////////////////////////////////////////////////////////////////////

class Face : public GrObj, public PropFriend<Face> {
public:
  //////////////
  // creators //
  //////////////

  Face (double eh=0.6, double es=0.4, double nl=0.4, double mh=-0.8, int categ=0);
  Face (istream &is, IOFlag flag);
  virtual ~Face ();

  virtual void serialize(ostream &os, IOFlag flag) const;
  virtual void deserialize(istream &is, IOFlag flag);
  // These functions write/read the object's state from/to an
  // output/input stream. The stream must already be connected to an
  // appropriate file or other source. The format used is:
  //
  // [Face] category eyeHgt eyeDist noseLen mouthHgt
  
  virtual int charCount() const;

  virtual void readFrom(Reader* reader);
  virtual void writeTo(Writer* writer) const;

  ////////////////
  // properties //
  ////////////////

  typedef PropertyInfo<Face> PInfo;
  static const vector<PInfo>& getPropertyInfos();

  CTProperty<Face, int> category;

  CTProperty<Face, double> eyeHeight;
  CTProperty<Face, double> eyeDistance;
  CTProperty<Face, double> noseLength;
  CTProperty<Face, double> mouthHeight;

  ///////////////
  // accessors //
  ///////////////

  virtual int getCategory() const { return category.getNative(); }

protected:
  virtual const double* getCtrlPnts() const;
  // Returns an array of Bezier control points for face outline

  virtual double getEyeAspect() const;
  // Returns the aspect ratio of eye outline.
  
  virtual double getVertOffset() const;
  // Returns the amount of vertical offset applied to all features.

  //////////////////
  // manipulators //
  //////////////////

public:
  virtual void setCategory(int val) { category.setNative(val); }

protected:
  virtual bool grGetBoundingBox(double& left, double& top,
										  double& right, double& bottom,
										  int& border_pixels) const;

  virtual void grRender() const; 
  // This overrides GrObj pure virtual function. It renders a face
  // with the appropriate parameters.

private:
  bool check() const;
  // Check all invariants and return true if everything is OK.

  Face(const Face&);            // copy constructor not to be used
  Face& operator=(const Face&); // assignment operator not to be used

  void makeIoList(vector<IO *>& vec);
  void makeIoList(vector<const IO *>& vec) const;
};

static const char vcid_face_h[] = "$Header$";
#endif // !FACE_H_DEFINED
