///////////////////////////////////////////////////////////////////////
//
// morphyface.h
// Rob Peters 
// created: Wed Sep  8 15:37:45 1999
// written: Wed Sep 29 13:25:32 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef MORPHYFACE_H_DEFINED
#define MORPHYFACE_H_DEFINED

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

#ifdef PROPERTY
#error PROPERTY macro already defined
#else
#define PROPERTY(type, name) \
  private: IoWrapper<type> its##name; \
  public:  type get##name() const { return its##name(); } \
  public:  void set##name(type val) { its##name() = val; sendStateChangeMsg(); }
#endif

#ifdef BOUNDED_PROPERTY
#error BOUNDED_PROPERTY macro already defined
#else
#define BOUNDED_PROPERTY(type, name, min, max) \
  private: IoWrapper<type> its##name; \
  public:  type get##name() const { return its##name(); } \
  public:  void set##name(type val) { \
				  if (val >= min && val <= max) its##name() = val; \
				  sendStateChangeMsg(); }
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

  MorphyFace(int categ=0);
  MorphyFace(istream &is, IOFlag flag);
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

  virtual bool grGetBoundingBox(double& left, double& top,
										  double& right, double& bottom,
										  int& border_pixels) const;
  ////////////////
  // properties //
  ////////////////

  PROPERTY(int, Category);	  // holds an arbitrary category specification

  PROPERTY(double, FaceWidth);
  PROPERTY(double, TopWidth);
  PROPERTY(double, BottomWidth);
  PROPERTY(double, TopHeight);
  PROPERTY(double, BottomHeight);

  PROPERTY(double, HairWidth);
  PROPERTY(int, HairStyle);

  PROPERTY(double, EyeYpos);
  PROPERTY(double, EyeDistance);
  PROPERTY(double, EyeHeight);
  PROPERTY(double, EyeAspectRatio);

  PROPERTY(double, PupilXpos);
  PROPERTY(double, PupilYpos);
  PROPERTY(double, PupilSize);
  BOUNDED_PROPERTY(double, PupilDilation, 0.0, 0.999);

  PROPERTY(double, EyebrowXpos);
  PROPERTY(double, EyebrowYpos);
  PROPERTY(double, EyebrowCurvature);
  PROPERTY(double, EyebrowAngle);
  PROPERTY(double, EyebrowThickness);

  PROPERTY(double, NoseXpos);
  PROPERTY(double, NoseYpos);
  PROPERTY(double, NoseLength);
  PROPERTY(double, NoseWidth);

  PROPERTY(double, MouthXpos);
  PROPERTY(double, MouthYpos);
  PROPERTY(double, MouthWidth);
  PROPERTY(double, MouthCurvature);

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
};

#undef PROPERTY

static const char vcid_morphyface_h[] = "$Header$";
#endif // !MORPHYFACE_H_DEFINED
