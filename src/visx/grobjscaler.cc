///////////////////////////////////////////////////////////////////////
//
// grobjscaler.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Jul 18 18:01:45 2001
// written: Mon Sep 10 17:17:39 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GROBJSCALER_CC_DEFINED
#define GROBJSCALER_CC_DEFINED

#include "visx/grobjscaler.h"

#include "gfx/canvas.h"
#include "gfx/vec3.h"

#include "util/trace.h"

void GrObjScaler::doScaling(Gfx::Canvas& canvas) const
{
DOTRACE("GrObjScaler::doScaling");
  if (Gmodes::NATIVE_SCALING == itsMode) return;

  Gfx::Vec3<double> vec(itsWidthFactor, itsHeightFactor, 1.0);
  canvas.scale(vec);
}

void GrObjScaler::setMode(Gmodes::ScalingMode new_mode)
{
DOTRACE("GrObjScaler::setMode");

  itsMode = new_mode;
}

void GrObjScaler::gnodeDraw(Gfx::Canvas& canvas) const
{
  Gfx::Canvas::MatrixSaver state(canvas);

  doScaling(canvas);

  child()->gnodeDraw(canvas);
}

Gfx::Rect<double> GrObjScaler::gnodeBoundingBox(Gfx::Canvas& canvas) const
{
  Gfx::Rect<double> bounds = child()->gnodeBoundingBox(canvas);

  bounds.scale(Gfx::Vec2<double>(itsWidthFactor, itsHeightFactor));

  return bounds;
}

static const char vcid_grobjscaler_cc[] = "$Header$";
#endif // !GROBJSCALER_CC_DEFINED
