///////////////////////////////////////////////////////////////////////
// face.h
// Rob Peters 
// created: Dec-98
// written: Thu Sep 23 11:35:46 1999
// $Id$
//
// The Face class is derived from GrObj. Face provides the
// functionality needed to display Brunswick faces parameterized by
// nose length, mouth height, eye height, and eye separation.
///////////////////////////////////////////////////////////////////////

#ifndef FACE_H_DEFINED
#define FACE_H_DEFINED

#include <cmath>					  // for abs()

#ifndef IOSTL_H_DEFINED
#include "iostl.h"
#endif

#ifndef VECTOR_DEFINED
#include <vector>
#define VECTOR_DEFINED
#endif

#ifndef GROBJ_H_DEFINED
#include "grobj.h"
#endif

///////////////////////////////////////////////////////////////////////
//
// Face class declaration
//
///////////////////////////////////////////////////////////////////////

class Face : public GrObj {
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

  ///////////////
  // accessors //
  ///////////////

  double getEyeHgt() const { return itsEyeHeight(); }

  double getEyeDist() const { return itsEyeDistance(); }

  double getNoseLen() const { return itsNoseLength(); }

  double getMouthHgt() const { return itsMouthHeight(); }
  
  virtual int getCategory() const { return itsCategory(); }

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
  void setEyeHgt(double eh);
  void setEyeDist(double ed);
  void setNoseLen(double nl);
  void setMouthHgt(double mh);
  
  virtual void setCategory(int val) { itsCategory() = val; }

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

  IoWrapper<double> itsEyeDistance;
  IoWrapper<double> itsNoseLength;
  IoWrapper<double> itsMouthHeight;
  IoWrapper<double> itsEyeHeight;

  IoWrapper<int> itsCategory;	  // holds an arbitrary category specification
};

///////////////////////////////////////////////////////////////////////
//
// Face inline member functions
//
///////////////////////////////////////////////////////////////////////

inline void Face::setNoseLen(double nl) {
  itsNoseLength() = abs(nl);
  sendStateChangeMsg();
}

inline void Face::setEyeDist(double ed) {
  itsEyeDistance() = abs(ed);
  sendStateChangeMsg();
}

inline void Face::setEyeHgt(double eh) {
  itsEyeHeight() = eh;
  sendStateChangeMsg();
}

inline void Face::setMouthHgt(double mh) {
  itsMouthHeight() = mh;
  sendStateChangeMsg();
}

static const char vcid_face_h[] = "$Header$";
#endif // !FACE_H_DEFINED
