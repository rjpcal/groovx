///////////////////////////////////////////////////////////////////////
// toglconfig.cc
// Rob Peters
// created: Wed Feb 24 10:18:17 1999
// written: Fri Mar 12 12:52:56 1999
static const char vcid_toglconfig_cc[] = "$Id$";
///////////////////////////////////////////////////////////////////////

#ifndef TOGLCONFIG_CC_DEFINED
#define TOGLCONFIG_CC_DEFINED

#include "toglconfig.h"

#include <cmath>

#include "gfxattribs.h"

ToglConfig::ToglConfig(Glist** glist, float dist, float unit_angle) :
  itsGlist_h(glist),
  itsViewingDistance(dist), 
  itsFixedScaleFlag(1) 
{ 
  setUnitAngle(unit_angle);
}

void ToglConfig::setViewingDistIn(float in) { 
  // according to similar triangles,
  //   new_dist / old_dist == new_scale / old_scale;
  float factor = in / itsViewingDistance;
  itsFixedScale *= factor;
  itsViewingDistance = in; 
}

void ToglConfig::setUnitAngle(float deg) {
  static const double deg_to_rad = 3.141593/180.0;
  itsFixedScaleFlag = 1;
  // tan(deg) == screen_unit_dist/viewing_dist;
  // screen_unit_dist == 1.0 * itsFixedScale / screepPpi;
  double screen_unit_dist = tan(deg*deg_to_rad) * itsViewingDistance;
  itsFixedScale = int(screen_unit_dist * GfxAttribs::getScreenPpi());
}

float ToglConfig::getFixedScale() const { 
  return itsFixedScale; 
}

void ToglConfig::setFixedScale(float s) { 
  itsFixedScaleFlag = 1; 
  itsFixedScale = s; 
}

rect ToglConfig::getMinRect() const { 
  return itsMinRect; 
}

void ToglConfig::setMinRectLTRB(float L, float T, float R, float B) { 
  itsFixedScaleFlag = 0; 
  itsMinRect.setRectLTRB(L,T,R,B);
}

int ToglConfig::usingFixedScale() const { 
  return itsFixedScaleFlag; 
}

Glist** ToglConfig::getGlist() const { 
  return itsGlist_h; 
}

#endif // !TOGLCONFIG_CC_DEFINED
