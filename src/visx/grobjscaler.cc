///////////////////////////////////////////////////////////////////////
//
// grobjscaler.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Jul 18 18:01:45 2001
// written: Wed Nov 13 10:55:11 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GROBJSCALER_CC_DEFINED
#define GROBJSCALER_CC_DEFINED

#include "visx/grobjscaler.h"

#include "gfx/canvas.h"

#include "gx/vec3.h"

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

void GrObjScaler::draw(Gfx::Canvas& canvas) const
{
  Gfx::MatrixSaver state(canvas);

  doScaling(canvas);

  child()->draw(canvas);
}

void GrObjScaler::getBoundingCube(Gfx::Box<double>& cube,
                                  Gfx::Canvas& canvas) const
{
  child()->getBoundingCube(cube, canvas);

  cube.scale(Gfx::Vec3<double>(itsWidthFactor, itsHeightFactor, 1.0));
}

static const char vcid_grobjscaler_cc[] = "$Header$";
#endif // !GROBJSCALER_CC_DEFINED
