///////////////////////////////////////////////////////////////////////
//
// grobjscaler.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Jul 18 18:01:45 2001
// written: Fri Aug 10 18:42:18 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GROBJSCALER_CC_DEFINED
#define GROBJSCALER_CC_DEFINED

#include "grobjscaler.h"

#include "gfx/canvas.h"
#include "gfx/vec3.h"

#include "util/trace.h"

void GrObjScaler::doScaling(Gfx::Canvas& canvas) const
{
DOTRACE("GrObjScaler::doScaling");
  if (Gmodes::NATIVE_SCALING == itsMode) return;

  Vec3<double> vec(itsWidthFactor, itsHeightFactor, 1.0);
  canvas.scale(vec);
}

void GrObjScaler::setMode(Gmodes::ScalingMode new_mode)
{
DOTRACE("GrObjScaler::setMode");
  if (itsMode == new_mode) return;

  itsMode = new_mode;

  if (itsMode == Gmodes::NATIVE_SCALING)
    {
      itsWidthFactor = 1.0;
      itsHeightFactor = 1.0;
    }
}

void GrObjScaler::gnodeDraw(Gfx::Canvas& canvas) const
{
  Gfx::Canvas::StateSaver state(canvas);

  doScaling(canvas);

  child()->gnodeDraw(canvas);
}

void GrObjScaler::gnodeUndraw(Gfx::Canvas& canvas) const
{
  Gfx::Canvas::StateSaver state(canvas);

  doScaling(canvas);

  child()->gnodeUndraw(canvas);
}

Rect<double> GrObjScaler::gnodeBoundingBox(Gfx::Canvas& canvas) const
{
  Rect<double> bounds = child()->gnodeBoundingBox(canvas);

  bounds.scale(Point<double>(itsWidthFactor, itsHeightFactor));

  return bounds;
}

static const char vcid_grobjscaler_cc[] = "$Header$";
#endif // !GROBJSCALER_CC_DEFINED
