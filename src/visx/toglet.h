///////////////////////////////////////////////////////////////////////
//
// toglconfig.h
// Rob Peters 
// created: Jan-99
// written: Thu May 27 20:22:52 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TOGLCONFIG_H_DEFINED
#define TOGLCONFIG_H_DEFINED

#ifndef RECT_H_DEFINED
#include "rect.h"
#endif

class Tlist;

///////////////////////////////////////////////////////////////////////
//
// ToglConfig class definition
//
///////////////////////////////////////////////////////////////////////

class ToglConfig {
public:
  ToglConfig(const Tlist& tlist, float dist, float unit_angle);
  virtual ~ToglConfig() {}

  void setViewingDistIn(float in);

  void setUnitAngle(float deg);

  float getFixedScale() const;
  void setFixedScale(float s);

  Rect<float> getMinRect() const;
  void setMinRectLTRB(float L, float T, float R, float B);

  bool usingFixedScale() const;

  const Tlist& getTlist() const;

private:
  ToglConfig(const ToglConfig&); // no copy constructor
  ToglConfig& operator=(const ToglConfig&); // no assignment operator

  const Tlist& itsTlist;
  float itsViewingDistance;     // inches
  bool itsFixedScaleFlag;
  float itsFixedScale;
  Rect<float> itsMinRect;
};

static const char vcid_toglconfig_h[] = "$Header$";
#endif // !TOGLCONFIG_H_DEFINED
