///////////////////////////////////////////////////////////////////////
//
// grobjaligner.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Jul 18 15:48:47 2001
// written: Mon Aug 13 12:17:29 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GROBJALIGNER_CC_DEFINED
#define GROBJALIGNER_CC_DEFINED

#include "grobjaligner.h"

#include "gfx/canvas.h"
#include "gfx/vec3.h"
#include "gfx/rect.h"

#include "util/trace.h"

void GrObjAligner::doAlignment(Gfx::Canvas& canvas,
                               const Gfx::Rect<double>& native) const
{
DOTRACE("GrObjAligner::doAlignment");

  if (Gmodes::NATIVE_ALIGNMENT == itsMode) return;

  // This indicates where the center of the object will go
  Gfx::Vec2<double> center = getCenter(native.width(), native.height());

  center -= native.center();

  Gfx::Vec3<double> vec(center.x(), center.y(), 0.0);

  canvas.translate(vec);
}


void GrObjAligner::gnodeDraw(Gfx::Canvas& canvas) const
{
  Gfx::Canvas::StateSaver state(canvas);

  doAlignment(canvas, child()->gnodeBoundingBox(canvas));

  child()->gnodeDraw(canvas);
}

void GrObjAligner::gnodeUndraw(Gfx::Canvas& canvas) const
{
  Gfx::Canvas::StateSaver state(canvas);

  doAlignment(canvas, child()->gnodeBoundingBox(canvas));

  child()->gnodeUndraw(canvas);
}

Gfx::Rect<double> GrObjAligner::gnodeBoundingBox(Gfx::Canvas& canvas) const
{
  Gfx::Rect<double> bounds = child()->gnodeBoundingBox(canvas);

  Gfx::Vec2<double> center = getCenter(bounds.width(), bounds.height());

  bounds.setCenter(center);

  return bounds;
}


static const char vcid_grobjaligner_cc[] = "$Header$";
#endif // !GROBJALIGNER_CC_DEFINED
