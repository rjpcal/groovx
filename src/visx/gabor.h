///////////////////////////////////////////////////////////////////////
//
// gabor.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Oct  6 10:45:58 1999
// written: Wed Oct  6 21:20:06 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GABOR_H_DEFINED
#define GABOR_H_DEFINED

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

class Gabor : public GrObj, public PropFriend<Gabor> {
public:
  Gabor();
  virtual ~Gabor();

  virtual void serialize(ostream &os, IOFlag flag) const;
  virtual void deserialize(istream &is, IOFlag flag);

  virtual int charCount() const;

  typedef int ColorMode;
  static const ColorMode GRAYSCALE = 1;
  static const ColorMode BW_DITHER_POINT = 2;
  static const ColorMode BW_DITHER_RECT = 3;

  typedef PropertyInfo<Gabor> PInfo;
  static const vector<PInfo>& getPropertyInfos();

  CTProperty<Gabor, ColorMode> colorMode;

  CTProperty<Gabor, double> contrast;

  CTProperty<Gabor, double> spatialFreq; // in cycles per unit
  CTProperty<Gabor, int> phase;          // in degrees

  CTProperty<Gabor, double> sigma;
  CTProperty<Gabor, double> aspectRatio; // width/height

  CTProperty<Gabor, int> orientation; // in degrees

  CTProperty<Gabor, int> resolution;     // subdivisions per unit
  CTProperty<Gabor, int> pointSize;

protected:
  virtual bool grGetBoundingBox(double& left, double& top,
										  double& right, double& bottom,
										  int& border_pixels) const;

  virtual void grRender() const;
};

static const char vcid_gabor_h[] = "$Header$";
#endif // !GABOR_H_DEFINED
