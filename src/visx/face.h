///////////////////////////////////////////////////////////////////////
// face.h
// Rob Peters 
// created: Dec-98
// written: Sun Mar 14 18:17:36 1999
//
// The Face class is a derived class of Scaleable (itself derived from
// GrObj). As such, Face*'s may be stored and manipulated in ObjList's
// and Glist's. The Face class provides the functionality needed to
// display Brunswick faces parameterized by nose length, mouth height,
// eye height, and eye separation.
///////////////////////////////////////////////////////////////////////

#ifndef FACE_H_DEFINED
#define FACE_H_DEFINED

#include <cmath>

#ifndef GROBJ_H_INCLUDED
#include "grobj.h"
#endif

///////////////////////////////////////////////////////////////////////
// Face class declaration
///////////////////////////////////////////////////////////////////////

class Face : public GrObj {
public:
  Face (float eh, float es, float nl, float mh, int categ=0);
  Face (istream &is, IOFlag flag);
  virtual ~Face ();

  // write/read the object's state from/to an output/input stream
  virtual IOResult serialize(ostream &os, IOFlag flag = NO_FLAGS) const;
  virtual IOResult deserialize(istream &is, IOFlag flag = NO_FLAGS);
  
  virtual void grRecompile() const; // overrides GrObj pure virtual function

  float getEyeHgt() const { return itsEyeheight; }
  float getEyeDist() const { return (itsEye[1] - itsEye[0]); }
  float getNoseLen() const { return (itsNose[1] - itsNose[0]); }
  float getMouthHgt() const { return itsMouthHeight; }
  
  virtual int getCategory() const { return itsCategory; }

  void setEyeHgt(float eh);
  void setEyeDist(float ed);
  void setNoseLen(float nl);
  void setMouthHgt(float mh);
  
  virtual void setCategory(int val) { itsCategory = val; }

private:
  Face(const Face&);				  // copy constructor not to be used
  Face& operator=(const Face&); // assignment operator not to be used

  float itsColor[3];				  // R,G,B colors
  float itsEye[2];				  // x positions for left, right eyes
  float itsNose[2];				  // y positions for bottom, top of nose
  float itsMouthHeight;			  // mouth height
  float itsEyeheight;			  // eyes height

  int itsCategory;
};

///////////////////////////////////////////////////////////////////////
// Face inline member functions
///////////////////////////////////////////////////////////////////////

inline void Face::setNoseLen(float nl) {
  grPostRecompile();
  itsNose[0] = -abs(nl)/2.0;	  // bottom
  itsNose[1] = -itsNose[0];	  // top
}

inline void Face::setEyeDist(float ed) {
  grPostRecompile();
  itsEye[0] = -abs(ed)/2.0;	  // left
  itsEye[1] = -itsEye[0];		  // right
}

inline void Face::setEyeHgt(float eh) {
  grPostRecompile();
  itsEyeheight = eh;
}

inline void Face::setMouthHgt(float mh) {
  grPostRecompile();
  itsMouthHeight = mh;
}

static const char vcid_face_h[] = "$Id$";
#endif // !FACE_H_DEFINED
