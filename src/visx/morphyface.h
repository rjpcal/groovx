///////////////////////////////////////////////////////////////////////
//
// morphyface.h
// Rob Peters 
// created: Wed Sep  8 15:37:45 1999
// written: Mon Nov 15 15:49:23 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef MORPHYFACE_H_DEFINED
#define MORPHYFACE_H_DEFINED

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
// MorphyFace class declaration
//
///////////////////////////////////////////////////////////////////////

class MorphyFace : public GrObj, public PropFriend<MorphyFace> {
public:
  //////////////
  // creators //
  //////////////

  MorphyFace();
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

  virtual void readFrom(Reader* reader);
  virtual void writeTo(Writer* writer) const;

  ////////////////
  // properties //
  ////////////////

  typedef PropertyInfo<MorphyFace> PInfo;
  static const vector<PInfo>& getPropertyInfos();

  CTProperty<MorphyFace, int> category;

  virtual int getCategory() const { return category.getNative(); }
  virtual void setCategory(int val) { category.setNative(val); }
  
  CTProperty<MorphyFace, double> faceWidth;
  CTProperty<MorphyFace, double> topWidth;
  CTProperty<MorphyFace, double> bottomWidth;
  CTProperty<MorphyFace, double> topHeight;
  CTProperty<MorphyFace, double> bottomHeight;

  CTProperty<MorphyFace, double> hairWidth;
  CTProperty<MorphyFace, int> hairStyle;

  CTProperty<MorphyFace, double> eyeYpos;
  CTProperty<MorphyFace, double> eyeDistance;
  CTProperty<MorphyFace, double> eyeHeight;
  CTProperty<MorphyFace, double> eyeAspectRatio;

  CTProperty<MorphyFace, double> pupilXpos;
  CTProperty<MorphyFace, double> pupilYpos;
  CTProperty<MorphyFace, double> pupilSize;
  CTBoundedProperty<MorphyFace, double, 0, 999, 1000> pupilDilation;

  CTProperty<MorphyFace, double> eyebrowXpos;
  CTProperty<MorphyFace, double> eyebrowYpos;
  CTProperty<MorphyFace, double> eyebrowCurvature;
  CTProperty<MorphyFace, double> eyebrowAngle;
  CTProperty<MorphyFace, double> eyebrowThickness;

  CTProperty<MorphyFace, double> noseXpos;
  CTProperty<MorphyFace, double> noseYpos;
  CTProperty<MorphyFace, double> noseLength;
  CTProperty<MorphyFace, double> noseWidth;

  CTProperty<MorphyFace, double> mouthXpos;
  CTProperty<MorphyFace, double> mouthYpos;
  CTProperty<MorphyFace, double> mouthWidth;
  CTProperty<MorphyFace, double> mouthCurvature;

protected:
  virtual bool grGetBoundingBox(Rect<double>& bbox,
										  int& border_pixels) const;
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

static const char vcid_morphyface_h[] = "$Header$";
#endif // !MORPHYFACE_H_DEFINED
