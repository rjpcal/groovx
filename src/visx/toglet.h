///////////////////////////////////////////////////////////////////////
//
// toglconfig.h
// Rob Peters 
// created: Jan-99
// written: Thu Jun 17 20:22:04 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TOGLCONFIG_H_DEFINED
#define TOGLCONFIG_H_DEFINED

#ifndef RECT_H_DEFINED
#include "rect.h"
#endif

///////////////////////////////////////////////////////////////////////
//
// ToglConfig class definition
//
///////////////////////////////////////////////////////////////////////

class ToglConfig {
public:
  ToglConfig(double dist, double unit_angle);
  virtual ~ToglConfig() {}

  void setViewingDistIn(double in);

  void setUnitAngle(double deg);

  double getFixedScale() const;
  void setFixedScale(double s);

  Rect<double> getMinRect() const;
  void setMinRectLTRB(double L, double T, double R, double B);

  bool usingFixedScale() const;

private:
  ToglConfig(const ToglConfig&); // no copy constructor
  ToglConfig& operator=(const ToglConfig&); // no assignment operator

  double itsViewingDistance;     // inches
  bool itsFixedScaleFlag;
  double itsFixedScale;
  Rect<double> itsMinRect;
};

static const char vcid_toglconfig_h[] = "$Header$";
#endif // !TOGLCONFIG_H_DEFINED
