///////////////////////////////////////////////////////////////////////
//
// grobjscaler.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Jul 18 18:00:57 2001
// written: Wed Jul 18 18:34:28 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GROBJSCALER_H_DEFINED
#define GROBJSCALER_H_DEFINED

#include "grobj.h"

class GrObjScaler {
public:
  GrObjScaler() :
    itsMode(GrObj::NATIVE_SCALING),
    itsWidthFactor(1.0),
    itsHeightFactor(1.0)
  {}

  void doScaling() const;
  GrObj::ScalingMode getMode() const { return itsMode; }
  void setMode(GrObj::ScalingMode new_mode);

  void setWidth(double new_width, double native_width)
  {
    double current_width = native_width * itsWidthFactor;

    if (new_width == 0.0 || new_width == current_width) return;
    if (itsMode == GrObj::NATIVE_SCALING) return;

    double new_width_factor = new_width / native_width;

    double change_factor = new_width_factor / itsWidthFactor;

    itsWidthFactor = new_width_factor;

    if (itsMode == GrObj::MAINTAIN_ASPECT_SCALING)
      {
        itsHeightFactor *= change_factor;
      }
  }

  void setHeight(double new_height, double native_height)
  {
    double current_height = native_height * itsHeightFactor;

    if (new_height == 0.0 || new_height == current_height) return;
    if (itsMode == GrObj::NATIVE_SCALING) return;

    double new_height_factor = new_height / native_height;

    double change_factor = new_height_factor / itsHeightFactor;

    itsHeightFactor = new_height_factor;

    if (itsMode == GrObj::MAINTAIN_ASPECT_SCALING)
      {
        itsWidthFactor *= change_factor;
      }
  }

  void setAspectRatio(double new_aspect_ratio)
  {
    double current = aspectRatio();

    if (new_aspect_ratio == 0.0 || new_aspect_ratio == current) return;
    if (itsMode == GrObj::NATIVE_SCALING) return;

    double change_factor = new_aspect_ratio / current;

    // By convention, we change only the width to reflect the change in
    // aspect ratio

    itsWidthFactor *= change_factor;
  }

  void setMaxDimension(double new_max_dimension, double current_max)
  {
    if (itsMode == GrObj::NATIVE_SCALING) return;

    double scaling_factor = new_max_dimension / current_max;

    itsWidthFactor *= scaling_factor;
    itsHeightFactor *= scaling_factor;
  }

  double aspectRatio() const
  {
    return (itsHeightFactor != 0.0 ? itsWidthFactor/itsHeightFactor : 0.0);
  }

private:
  GrObj::ScalingMode itsMode;
public:
  double itsWidthFactor;
  double itsHeightFactor;
};

static const char vcid_grobjscaler_h[] = "$Header$";
#endif // !GROBJSCALER_H_DEFINED
