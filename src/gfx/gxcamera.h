///////////////////////////////////////////////////////////////////////
//
// gxcamera.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Nov 21 15:18:58 2002
// written: Thu Nov 21 15:26:13 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXCAMERA_H_DEFINED
#define GXCAMERA_H_DEFINED

#include "gx/rect.h"

// ####################################################################

/// GxCamera sets up an orthographic or perspective projection.

class GxCamera
{
public:
  enum ResizePolicy
    {
      FIXED_SCALE,
      FIXED_RECT,
      MIN_RECT,
      PERSPECTIVE,
    };

  GxCamera(double dist = 30.0) :
    itsPolicy(FIXED_SCALE),
    itsViewingDistance(dist),
    itsPixelsPerUnit(1.0),
    itsRect()
  {}

  // For FIXED_SCALE mode:
  void setPixelsPerUnit(double s);
  void setUnitAngle(double deg, double screen_ppi);
  void setViewingDistIn(double inches);
  bool usingFixedScale() const { return itsPolicy == FIXED_SCALE; }

  // For FIXED_RECT mode:
  void setFixedRectLTRB(double L, double T, double R, double B);

  // For MIN_RECT mode:
  void setMinRectLTRB(double L, double T, double R, double B);

  // For FIXED_RECT or MIN_RECT modes:
  void scaleRect(double factor);

  // For PERSPECTIVE modes:
  void setPerspective(double fovy, double zNear, double zFar);

  void reconfigure(const int width, const int height);

private:
  ResizePolicy itsPolicy;
  double itsViewingDistance;    // inches
  double itsPixelsPerUnit;      // pixels per GLunit
  Gfx::Rect<double> itsRect;

  struct Perspective
  {
    double fovy;
    double zNear;
    double zFar;
  };

  Perspective itsPerspective;
};

static const char vcid_gxcamera_h[] = "$Header$";
#endif // !GXCAMERA_H_DEFINED
