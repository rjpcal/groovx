///////////////////////////////////////////////////////////////////////
// morphyface.h
// Rob Peters 
// created: Wed Sep  8 15:37:45 1999
// written: Thu Sep  9 18:31:10 1999
// $Id$
//
// The MorphyFace class is derived from GrObj. MorphyFace provides the
// functionality needed to display Brunswick faces parameterized by
// nose length, mouth height, eye height, and eye separation.
///////////////////////////////////////////////////////////////////////

#ifndef MORPHYFACE_H_DEFINED
#define MORPHYFACE_H_DEFINED

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
// MorphyFace class declaration
//
///////////////////////////////////////////////////////////////////////

class MorphyFace : public GrObj {
public:
  //////////////
  // creators //
  //////////////

  MorphyFace (double eh=0.6, double es=0.4, double nl=0.4, double mh=-0.8, int categ=0);
  MorphyFace (istream &is, IOFlag flag);
  virtual ~MorphyFace ();

  virtual void serialize(ostream &os, IOFlag flag) const;
  virtual void deserialize(istream &is, IOFlag flag);
  // These functions write/read the object's state from/to an
  // output/input stream. The stream must already be connected to an
  // appropriate file or other source. The format used is:
  //
  // [MorphyFace] category eyeHgt eyeDist noseLen mouthHgt
  
  virtual int charCount() const;

  ///////////////
  // accessors //
  ///////////////

  double getEyeYpos() const { return itsEyeYpos(); }
  double getEyeDist() const { return itsEyeDistance(); }
  double getNoseLen() const { return itsNoseLength(); }
  double getEyeAspectRatio() const { return itsEyeAspectRatio(); }
  double getFaceWidth() const { return itsFaceWidth(); }
  double getTopWidth() const { return itsTopWidth(); }
  double getBottomWidth() const { return itsBottomWidth(); }
  double getTopHeight() const { return itsTopHeight(); }
  double getBottomHeight() const { return itsBottomHeight(); }

  double getMouthWidth() const { return itsMouthWidth(); }
  double getPupilSize() const { return itsPupilSize(); }
  double getPupilDilation() const { return itsPupilDilation(); }

  double getMouthCurvature() const { return itsMouthCurvature(); }
  double getMouthXpos() const { return itsMouthXpos(); }
  double getMouthYpos() const { return itsMouthYpos(); }
  double getNoseWidth() const { return itsNoseWidth(); }
  double getNoseXpos() const { return itsNoseXpos(); }
  double getNoseYpos() const { return itsNoseYpos(); }

  double getEyeHeight() const { return itsEyeHeight(); }
  double getPupilXpos() const { return itsPupilXpos(); }
  double getPupilYpos() const { return itsPupilYpos(); }
  double getEyebrowXpos() const { return itsEyebrowXpos(); }
  double getEyebrowYpos() const { return itsEyebrowYpos(); }
  double getEyebrowCurvature() const { return itsEyebrowCurvature(); }
  double getEyebrowAngle() const { return itsEyebrowAngle(); }
  double getEyebrowThickness() const { return itsEyebrowThickness(); }

  virtual int getCategory() const { return itsCategory(); }

  //////////////////
  // manipulators //
  //////////////////

public:
  void setEyeYpos(double eh);
  void setEyeDist(double ed);
  void setNoseLen(double nl);
  void setEyeAspectRatio(double val);
  void setFaceWidth(double val);
  void setTopWidth(double val);
  void setBottomWidth(double val);
  void setTopHeight(double val);
  void setBottomHeight(double val);

  void setMouthWidth(double val);
  void setPupilSize(double val);
  void setPupilDilation(double val);

  void setMouthCurvature(double val);
  void setMouthXpos(double val);
  void setMouthYpos(double val);
  void setNoseWidth(double val);
  void setNoseXpos(double val);
  void setNoseYpos(double val);

  void setEyeHeight(double val);
  void setPupilXpos(double val);
  void setPupilYpos(double val);
  void setEyebrowXpos(double val);
  void setEyebrowYpos(double val);
  void setEyebrowCurvature(double val);
  void setEyebrowAngle(double val);
  void setEyebrowThickness(double val);

  virtual void setCategory(int val) { itsCategory() = val; }

protected:
  virtual void grRender() const; 
  // This overrides GrObj pure virtual function. It renders a face
  // with the appropriate parameters.

private:
  bool check() const;
  // Check all invariants and return true if everything is OK.

  MorphyFace(const MorphyFace&);            // copy constructor not to be used
  MorphyFace& operator=(const MorphyFace&); // assignment operator not to be used

  void makeIoList(vector<IO *>& vec);
  void makeIoList(vector<const IO *>& vec) const;

/*
  IoWrapper<double> its\(.*\);

  IoWrapper<double> its\1;
  void set\1(double val);
  double get\1() const { return its\1(); }
inline void MorphyFace::set\1(double val) {
  its\1() = val;
  sendStateChangeMsg();
}
*/  

  IoWrapper<double> itsFaceWidth;
  IoWrapper<double> itsTopWidth;
  IoWrapper<double> itsBottomWidth;
  IoWrapper<double> itsTopHeight;
  IoWrapper<double> itsBottomHeight;

  IoWrapper<double> itsEyeYpos;
  IoWrapper<double> itsEyeDistance;
  IoWrapper<double> itsEyeHeight;
  IoWrapper<double> itsEyeAspectRatio;

  IoWrapper<double> itsPupilXpos;
  IoWrapper<double> itsPupilYpos;
  IoWrapper<double> itsPupilSize;
  IoWrapper<double> itsPupilDilation;

  IoWrapper<double> itsEyebrowXpos;
  IoWrapper<double> itsEyebrowYpos;
  IoWrapper<double> itsEyebrowCurvature;
  IoWrapper<double> itsEyebrowAngle;
  IoWrapper<double> itsEyebrowThickness;

  IoWrapper<double> itsNoseXpos;
  IoWrapper<double> itsNoseYpos;
  IoWrapper<double> itsNoseLength;
  IoWrapper<double> itsNoseWidth;

  IoWrapper<double> itsMouthXpos;
  IoWrapper<double> itsMouthYpos;
  IoWrapper<double> itsMouthWidth;
  IoWrapper<double> itsMouthCurvature;


  IoWrapper<int> itsCategory;	  // holds an arbitrary category specification
};

///////////////////////////////////////////////////////////////////////
//
// MorphyFace inline member functions
//
///////////////////////////////////////////////////////////////////////

inline void MorphyFace::setNoseLen(double nl) {
  itsNoseLength() = abs(nl);
  sendStateChangeMsg();
}

inline void MorphyFace::setEyeDist(double ed) {
  itsEyeDistance() = abs(ed);
  sendStateChangeMsg();
}

inline void MorphyFace::setEyeYpos(double eh) {
  itsEyeYpos() = eh;
  sendStateChangeMsg();
}

inline void MorphyFace::setEyeAspectRatio(double val) {
  itsEyeAspectRatio() = val;
  sendStateChangeMsg();
}
  
inline void MorphyFace::setFaceWidth(double val) {
  itsFaceWidth() = val;
  sendStateChangeMsg();
}

inline void MorphyFace::setTopWidth(double val) {
  itsTopWidth() = val;
  sendStateChangeMsg();
}
inline void MorphyFace::setBottomWidth(double val) {
  itsBottomWidth() = val;
  sendStateChangeMsg();
}

inline void MorphyFace::setTopHeight(double val) {
  itsTopHeight() = val;
  sendStateChangeMsg();
}

inline void MorphyFace::setBottomHeight(double val) {
  itsBottomHeight() = val;
  sendStateChangeMsg();
}

inline void MorphyFace::setMouthWidth(double val) {
  itsMouthWidth() = val;
  sendStateChangeMsg();
}

inline void MorphyFace::setPupilSize(double val) {
  itsPupilSize() = val;
  sendStateChangeMsg();
}

inline void MorphyFace::setPupilDilation(double val) {
  if ( (val >= 0.0) && (val <= 0.999) ) {
	 itsPupilDilation() = val;
	 sendStateChangeMsg();
  }
}


inline void MorphyFace::setMouthCurvature(double val) {
  itsMouthCurvature() = val;
  sendStateChangeMsg();
}

inline void MorphyFace::setMouthXpos(double val) {
  itsMouthXpos() = val;
  sendStateChangeMsg();
}

inline void MorphyFace::setMouthYpos(double val) {
  itsMouthYpos() = val;
  sendStateChangeMsg();
}

inline void MorphyFace::setNoseWidth(double val) {
  itsNoseWidth() = val;
  sendStateChangeMsg();
}

inline void MorphyFace::setNoseXpos(double val) {
  itsNoseXpos() = val;
  sendStateChangeMsg();
}

inline void MorphyFace::setNoseYpos(double val) {
  itsNoseYpos() = val;
  sendStateChangeMsg();
}

inline void MorphyFace::setEyeHeight(double val) {
  itsEyeHeight() = val;
  sendStateChangeMsg();
}

inline void MorphyFace::setPupilXpos(double val) {
  itsPupilXpos() = val;
  sendStateChangeMsg();
}

inline void MorphyFace::setPupilYpos(double val) {
  itsPupilYpos() = val;
  sendStateChangeMsg();
}

inline void MorphyFace::setEyebrowXpos(double val) {
  itsEyebrowXpos() = val;
  sendStateChangeMsg();
}

inline void MorphyFace::setEyebrowYpos(double val) {
  itsEyebrowYpos() = val;
  sendStateChangeMsg();
}

inline void MorphyFace::setEyebrowCurvature(double val) {
  itsEyebrowCurvature() = val;
  sendStateChangeMsg();
}

inline void MorphyFace::setEyebrowAngle(double val) {
  itsEyebrowAngle() = val;
  sendStateChangeMsg();
}

inline void MorphyFace::setEyebrowThickness(double val) {
  itsEyebrowThickness() = val;
  sendStateChangeMsg();
}

static const char vcid_morphyface_h[] = "$Header$";
#endif // !MORPHYFACE_H_DEFINED
