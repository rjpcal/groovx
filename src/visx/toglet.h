///////////////////////////////////////////////////////////////////////
// toglconfig.h
// Rob Peters 
// created: Jan-99
// written: Sat Mar 13 13:16:43 1999
///////////////////////////////////////////////////////////////////////

#ifndef TOGLCONFIG_H_DEFINED
#define TOGLCONFIG_H_DEFINED

#ifndef RECT_H_INCLUDED
#include "rect.h"
#endif

class Tlist;

class ToglConfig {
public:
  ToglConfig(Tlist** tlist, float dist, float unit_angle);
  virtual ~ToglConfig() {}

  void setViewingDistIn(float in);

  void setUnitAngle(float deg);

  float getFixedScale() const;
  void setFixedScale(float s);

  rect getMinRect() const;
  void setMinRectLTRB(float L, float T, float R, float B);

  int usingFixedScale() const;

  Tlist** getTlist() const;

private:
  ToglConfig(const ToglConfig&);	// no copy constructor
  ToglConfig& operator=(const ToglConfig&); // no assignment operator

  Tlist** itsTlist_h;            // handle to a Tlist
  float itsViewingDistance;	  // inches
  int itsFixedScaleFlag;
  float itsFixedScale;
  rect itsMinRect;
};

static const char vcid_toglconfig_h[] = "$Id$";
#endif // !TOGLCONFIG_H_DEFINED
