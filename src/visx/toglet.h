///////////////////////////////////////////////////////////////////////
// toglconfig.h
// Rob Peters 
// created: Jan-99
// written: Wed Mar 10 11:12:48 1999
///////////////////////////////////////////////////////////////////////

#ifndef TOGLCONFIG_H_DEFINED
#define TOGLCONFIG_H_DEFINED

#ifndef RECT_H_INCLUDED
#include "rect.h"
#endif

class Glist;

class ToglConfig {
public:
  ToglConfig(Glist** glist, float dist, float unit_angle);
  virtual ~ToglConfig() {}

  void setViewingDistIn(float in);

  void setUnitAngle(float deg);

  float getFixedScale() const;
  void setFixedScale(float s);

  rect getMinRect() const;
  void setMinRectLTRB(float L, float T, float R, float B);

  int usingFixedScale() const;

  Glist** getGlist() const;

private:
  ToglConfig(const ToglConfig&);	// no copy constructor
  ToglConfig& operator=(const ToglConfig&); // no assignment operator

  Glist** itsGlist_h;            // handle to a Glist
  float itsViewingDistance;	  // inches
  int itsFixedScaleFlag;
  float itsFixedScale;
  rect itsMinRect;
};

#endif // !TOGLCONFIG_H_DEFINED
