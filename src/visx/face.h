///////////////////////////////////////////////////////////////////////
// face.h
// Rob Peters 
// created: Dec-98
// written: Mon Apr 26 14:39:14 1999
// $Id$
//
// The Face class is derived from GrObj. As such, Face*'s may be
// stored and manipulated in ObjList's. The Face class provides the
// functionality needed to display Brunswick faces parameterized by
// nose length, mouth height, eye height, and eye separation.
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
  //////////////
  // creators //
  //////////////

  Face (float eh, float es, float nl, float mh, int categ=0);
  Face (istream &is, IOFlag flag);
  virtual ~Face ();

  virtual void serialize(ostream &os, IOFlag flag) const;
  virtual void deserialize(istream &is, IOFlag flag);
  // These functions write/read the object's state from/to an
  // output/input stream. The stream must already be connected to an
  // appropriate file or other source. The format used is:
  //
  // [Face] category eyeHgt eyeDist noseLen mouthHgr
  
  ///////////////
  // accessors //
  ///////////////

  float getEyeHgt() const { return itsEyeheight; }

  float getEyeDist() const { return (itsEye[1] - itsEye[0]); }
  // Always returns a non-negative value, since itsEye[1] is always
  // >=0.0 and itsEye[0] is always <= 0.0

  float getNoseLen() const { return (itsNose[1] - itsNose[0]); }

  float getMouthHgt() const { return itsMouthHeight; }
  
  virtual int getCategory() const { return itsCategory; }

  //////////////////
  // manipulators //
  //////////////////

  void setEyeHgt(float eh);
  void setEyeDist(float ed);
  void setNoseLen(float nl);
  void setMouthHgt(float mh);
  
  virtual void setCategory(int val) { itsCategory = val; }

protected:
  virtual void grRecompile() const; 
  // This overrides GrObj pure virtual function. It compiles an OpenGL
  // display list that, when called, will render a face with the
  // appropriate parameters.
  

private:
  bool check() const;
  // Check all invariants and return true if everything is OK.

  Face(const Face&);            // copy constructor not to be used
  Face& operator=(const Face&); // assignment operator not to be used

  float itsEye[2];              // x positions for left, right eyes
  float itsNose[2];             // y positions for bottom, top of nose
  float itsMouthHeight;         // mouth height
  float itsEyeheight;           // eyes height

  int itsCategory;				  // holds an arbitrary category specification
};

///////////////////////////////////////////////////////////////////////
// Face inline member functions
///////////////////////////////////////////////////////////////////////

inline void Face::setNoseLen(float nl) {
  grPostRecompile();
  itsNose[0] = -abs(nl)/2.0;    // bottom, always <= 0.0
  itsNose[1] = -itsNose[0];     // top, always >= 0.0
}

inline void Face::setEyeDist(float ed) {
  grPostRecompile();
  itsEye[0] = -abs(ed)/2.0;     // left, always <= 0.0
  itsEye[1] = -itsEye[0];       // right, always >= 0.0
}

inline void Face::setEyeHgt(float eh) {
  grPostRecompile();
  itsEyeheight = eh;
}

inline void Face::setMouthHgt(float mh) {
  grPostRecompile();
  itsMouthHeight = mh;
}

static const char vcid_face_h[] = "$Header$";
#endif // !FACE_H_DEFINED
