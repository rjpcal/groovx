///////////////////////////////////////////////////////////////////////
//
// grobjscaler.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Jul 18 18:01:45 2001
// written: Fri Aug 10 14:53:54 2001
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

  switch (new_mode) {
  case Gmodes::NATIVE_SCALING:
    itsMode = new_mode;

    itsWidthFactor = 1.0;
    itsHeightFactor = 1.0;
    break;

  case Gmodes::MAINTAIN_ASPECT_SCALING:
  case Gmodes::FREE_SCALING:
    itsMode = new_mode;
    break;

  } // end switch
}

static const char vcid_grobjscaler_cc[] = "$Header$";
#endif // !GROBJSCALER_CC_DEFINED
