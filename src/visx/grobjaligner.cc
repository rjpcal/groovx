///////////////////////////////////////////////////////////////////////
//
// grobjaligner.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Jul 18 15:48:47 2001
// written: Wed Nov 13 10:13:29 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GROBJALIGNER_CC_DEFINED
#define GROBJALIGNER_CC_DEFINED

#include "visx/grobjaligner.h"

#include "gfx/canvas.h"

#include "gx/rect.h"
#include "gx/vec3.h"

#include "util/trace.h"

void GrObjAligner::doAlignment(Gfx::Canvas& canvas,
                               const Gfx::Rect<double>& native) const
{
DOTRACE("GrObjAligner::doAlignment");

  if (Gmodes::NATIVE_ALIGNMENT == itsMode) return;

  // This indicates where the center of the object will go
  Gfx::Vec2<double> center = getCenter(native);

  center -= native.center();

  Gfx::Vec3<double> vec(center.x(), center.y(), 0.0);

  canvas.translate(vec);
}


void GrObjAligner::draw(Gfx::Canvas& canvas) const
{
  Gfx::MatrixSaver state(canvas);

  doAlignment(canvas, child()->gnodeBoundingBox(canvas));

  child()->draw(canvas);
}

Gfx::Rect<double> GrObjAligner::gnodeBoundingBox(Gfx::Canvas& canvas) const
{
  Gfx::Rect<double> bounds = child()->gnodeBoundingBox(canvas);

  Gfx::Vec2<double> center = getCenter(bounds);

  bounds.setCenter(center);

  return bounds;
}


static const char vcid_grobjaligner_cc[] = "$Header$";
#endif // !GROBJALIGNER_CC_DEFINED
