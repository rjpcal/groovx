///////////////////////////////////////////////////////////////////////
//
// grobjscaler.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Jul 18 18:00:57 2001
// written: Wed Nov 13 10:13:50 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GROBJSCALER_H_DEFINED
#define GROBJSCALER_H_DEFINED

#include "visx/gmodes.h"
#include "visx/gnode.h"

#include "gx/rect.h"

#include "util/algo.h"

class GrObjScaler : public Gnode
{
public:
  GrObjScaler(Util::SoftRef<Gnode> child) :
    Gnode(child),
    itsMode(Gmodes::NATIVE_SCALING),
    itsWidthFactor(1.0),
    itsHeightFactor(1.0)
  {}

  void doScaling(Gfx::Canvas& canvas) const;
  Gmodes::ScalingMode getMode() const { return itsMode; }
  void setMode(Gmodes::ScalingMode new_mode);

  void setWidth(Gfx::Canvas& canvas, double new_width)
  {
    Gfx::Rect<double> native_bbox = child()->gnodeBoundingBox(canvas);

    double current_width = native_bbox.width() * itsWidthFactor;

    if (new_width == 0.0 || new_width == current_width) return;
    if (itsMode == Gmodes::NATIVE_SCALING) return;

    double new_width_factor = new_width / native_bbox.width();

    double change_factor = new_width_factor / itsWidthFactor;

    itsWidthFactor = new_width_factor;

    if (itsMode == Gmodes::MAINTAIN_ASPECT_SCALING)
      {
        itsHeightFactor *= change_factor;
      }
  }

  void setHeight(Gfx::Canvas& canvas, double new_height)
  {
    Gfx::Rect<double> native_bbox = child()->gnodeBoundingBox(canvas);

    double current_height = native_bbox.height() * itsHeightFactor;

    if (new_height == 0.0 || new_height == current_height) return;
    if (itsMode == Gmodes::NATIVE_SCALING) return;

    double new_height_factor = new_height / native_bbox.height();

    double change_factor = new_height_factor / itsHeightFactor;

    itsHeightFactor = new_height_factor;

    if (itsMode == Gmodes::MAINTAIN_ASPECT_SCALING)
      {
        itsWidthFactor *= change_factor;
      }
  }

  void setAspectRatio(double new_aspect_ratio)
  {
    double current = aspectRatio();

    if (new_aspect_ratio == 0.0 || new_aspect_ratio == current) return;
    if (itsMode != Gmodes::FREE_SCALING) return;

    double change_factor = new_aspect_ratio / current;

    // By convention, we change only the width to reflect the change in
    // aspect ratio

    itsWidthFactor *= change_factor;
  }

  void setMaxDim(Gfx::Canvas& canvas, double new_max_dimension)
  {
    if (itsMode == Gmodes::NATIVE_SCALING) return;

    double scaling_factor = new_max_dimension / scaledMaxDim(canvas);

    itsWidthFactor *= scaling_factor;
    itsHeightFactor *= scaling_factor;
  }

  double aspectRatio() const
  {
    return itsMode != Gmodes::FREE_SCALING ? 1.0 :
      (itsHeightFactor != 0.0 ? itsWidthFactor/itsHeightFactor : 0.0);
  }

  double scaledWidth(Gfx::Canvas& canvas)
  {
    Gfx::Rect<double> native_bbox = child()->gnodeBoundingBox(canvas);
    return native_bbox.width() * itsWidthFactor;
  }

  double scaledHeight(Gfx::Canvas& canvas)
  {
    Gfx::Rect<double> native_bbox = child()->gnodeBoundingBox(canvas);
    return native_bbox.height() * itsHeightFactor;
  }

  double scaledMaxDim(Gfx::Canvas& canvas)
  {
    return Util::max(scaledWidth(canvas), scaledHeight(canvas));
  }

  virtual void draw(Gfx::Canvas& canvas) const;

  virtual Gfx::Rect<double> gnodeBoundingBox(Gfx::Canvas& canvas) const;

private:

  Gmodes::ScalingMode itsMode;

public:
  double itsWidthFactor;
  double itsHeightFactor;
};

static const char vcid_grobjscaler_h[] = "$Header$";
#endif // !GROBJSCALER_H_DEFINED
