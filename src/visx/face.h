///////////////////////////////////////////////////////////////////////
// face.h
// Rob Peters 
// created: Dec-98
// written: Sat May 15 15:01:41 1999
// $Id$
//
// The Face class is derived from GrObj. As such, Face*'s may be
// stored and manipulated in ObjList's. The Face class provides the
// functionality needed to display Brunswick faces parameterized by
// nose length, mouth height, eye height, and eye separation.
///////////////////////////////////////////////////////////////////////

#ifndef FACE_H_DEFINED
#define FACE_H_DEFINED

#include <cmath>					  // for abs()

#ifndef IOSTL_H_INCLUDED
#include "iostl.h"
#endif

#ifndef VECTOR_INCLUDED
#include <vector>
#endif

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

  Face (float eh=0.6, float es=0.4, float nl=0.4, float mh=-0.8, int categ=0);
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

  float getEyeHgt() const { return itsEyeHeight(); }

  float getEyeDist() const { return itsEyeDistance(); }

  float getNoseLen() const { return itsNoseLength(); }

  float getMouthHgt() const { return itsMouthHeight(); }
  
  virtual int getCategory() const { return itsCategory(); }

protected:
  virtual const float* getCtrlPnts() const;
  // Returns an array of Bezier control points for face outline

  virtual float getEyeAspect() const;
  // Returns the aspect ratio of eye outline.
  
  virtual float getVertOffset() const;
  // Returns the amount of vertical offset applied to all features.

  //////////////////
  // manipulators //
  //////////////////

public:
  void setEyeHgt(float eh);
  void setEyeDist(float ed);
  void setNoseLen(float nl);
  void setMouthHgt(float mh);
  
  virtual void setCategory(int val) { itsCategory() = val; }

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

  void makeIoList(vector<IO *>& vec);
  void makeIoList(vector<const IO *>& vec) const;

//   float itsEyeDistance;
//   float itsNoseLength;
//   float itsMouthHeight;         // mouth height
//   float itsEyeHeight;           // eyes height
  IoWrapper<float> itsEyeDistance;
  IoWrapper<float> itsNoseLength;
  IoWrapper<float> itsMouthHeight;
  IoWrapper<float> itsEyeHeight;

  IoWrapper<int> itsCategory;	  // holds an arbitrary category specification
//   int itsCategory;				  // holds an arbitrary category specification
};

///////////////////////////////////////////////////////////////////////
// Face inline member functions
///////////////////////////////////////////////////////////////////////

inline void Face::setNoseLen(float nl) {
  grPostRecompile();
  itsNoseLength() = abs(nl);
}

inline void Face::setEyeDist(float ed) {
  grPostRecompile();
  itsEyeDistance() = abs(ed);
}

inline void Face::setEyeHgt(float eh) {
  grPostRecompile();
  itsEyeHeight() = eh;
}

inline void Face::setMouthHgt(float mh) {
  grPostRecompile();
  itsMouthHeight() = mh;
}

static const char vcid_face_h[] = "$Header$";
#endif // !FACE_H_DEFINED
