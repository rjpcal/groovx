///////////////////////////////////////////////////////////////////////
//
// gxscaler.cc
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Nov 13 13:04:32 2002
// written: Thu Nov 14 17:12:00 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXSCALER_CC_DEFINED
#define GXSCALER_CC_DEFINED

#include "gxscaler.h"

#include "gfx/canvas.h"

#include "gx/box.h"
#include "gx/vec3.h"

#include "util/trace.h"

GxScaler::GxScaler() :
  GxBin(),
  itsMode(NATIVE_SCALING),
  itsWidthFactor(1.0),
  itsHeightFactor(1.0)
{}

GxScaler::GxScaler(Util::Ref<GxNode> child) :
  GxBin(child),
  itsMode(NATIVE_SCALING),
  itsWidthFactor(1.0),
  itsHeightFactor(1.0)
{}

GxScaler* GxScaler::make()
{
  return new GxScaler;
}

void GxScaler::setMode(Mode new_mode)
{
DOTRACE("GxScaler::setMode");

  itsMode = new_mode;
}

void GxScaler::setWidth(Gfx::Canvas& canvas, double new_width)
{
  Gfx::Rect<double> native_bbox = child()->getBoundingBox(canvas);

  double current_width = native_bbox.width() * itsWidthFactor;

  if (new_width == 0.0 || new_width == current_width) return;
  if (itsMode == NATIVE_SCALING) return;

  double new_width_factor = new_width / native_bbox.width();

  double change_factor = new_width_factor / itsWidthFactor;

  itsWidthFactor = new_width_factor;

  if (itsMode == MAINTAIN_ASPECT_SCALING)
    {
      itsHeightFactor *= change_factor;
    }
}

void GxScaler::setHeight(Gfx::Canvas& canvas, double new_height)
{
  Gfx::Rect<double> native_bbox = child()->getBoundingBox(canvas);

  double current_height = native_bbox.height() * itsHeightFactor;

  if (new_height == 0.0 || new_height == current_height) return;
  if (itsMode == NATIVE_SCALING) return;

  double new_height_factor = new_height / native_bbox.height();

  double change_factor = new_height_factor / itsHeightFactor;

  itsHeightFactor = new_height_factor;

  if (itsMode == MAINTAIN_ASPECT_SCALING)
    {
      itsWidthFactor *= change_factor;
    }
}

void GxScaler::setAspectRatio(double new_aspect_ratio)
{
  double current = aspectRatio();

  if (new_aspect_ratio == 0.0 || new_aspect_ratio == current) return;
  if (itsMode != FREE_SCALING) return;

  double change_factor = new_aspect_ratio / current;

  // By convention, we change only the width to reflect the change in
  // aspect ratio

  itsWidthFactor *= change_factor;
}

void GxScaler::setMaxDim(Gfx::Canvas& canvas, double new_max_dimension)
{
  if (itsMode == NATIVE_SCALING) return;

  double scaling_factor = new_max_dimension / scaledMaxDim(canvas);

  itsWidthFactor *= scaling_factor;
  itsHeightFactor *= scaling_factor;
}

double GxScaler::aspectRatio() const
{
  return itsMode != FREE_SCALING ? 1.0 :
    (itsHeightFactor != 0.0 ? itsWidthFactor/itsHeightFactor : 0.0);
}

double GxScaler::scaledWidth(Gfx::Canvas& canvas)
{
  Gfx::Rect<double> native_bbox = child()->getBoundingBox(canvas);
  return native_bbox.width() * itsWidthFactor;
}

double GxScaler::scaledHeight(Gfx::Canvas& canvas)
{
  Gfx::Rect<double> native_bbox = child()->getBoundingBox(canvas);
  return native_bbox.height() * itsHeightFactor;
}

double GxScaler::scaledMaxDim(Gfx::Canvas& canvas)
{
  return Util::max(scaledWidth(canvas), scaledHeight(canvas));
}

void GxScaler::draw(Gfx::Canvas& canvas) const
{
  if (NATIVE_SCALING == itsMode)
    {
      child()->draw(canvas);
    }
  else
    {
      Gfx::MatrixSaver state(canvas);
      canvas.scale(Gfx::Vec3<double>(itsWidthFactor, itsHeightFactor, 1.0));
      child()->draw(canvas);
    }
}

void GxScaler::getBoundingCube(Gfx::Box<double>& cube,
                               Gfx::Canvas& canvas) const
{
  child()->getBoundingCube(cube, canvas);

  cube.scale(Gfx::Vec3<double>(itsWidthFactor, itsHeightFactor, 1.0));
}

static const char vcid_gxscaler_cc[] = "$Header$";
#endif // !GXSCALER_CC_DEFINED
