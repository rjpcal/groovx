///////////////////////////////////////////////////////////////////////
//
// grobjaligner.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Jul 18 15:48:47 2001
// written: Wed Nov 13 12:46:45 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GROBJALIGNER_CC_DEFINED
#define GROBJALIGNER_CC_DEFINED

#include "visx/grobjaligner.h"

#include "gfx/canvas.h"

#include "gx/box.h"
#include "gx/rect.h"
#include "gx/vec3.h"

#include "util/trace.h"

GrObjAligner::GrObjAligner(Util::SoftRef<GxNode> child) :
  GxBin(child),
  itsMode(NATIVE_ALIGNMENT),
  itsCenter(0.0, 0.0)
{}

Gfx::Vec2<double> GrObjAligner::getCenter(const Gfx::Rect<double>& bounds) const
{
DOTRACE("GrObjAligner::getCenter");
  switch (itsMode)
    {
    case CENTER_ON_CENTER:
      return Gfx::Vec2<double>(0.0, 0.0);
    case NW_ON_CENTER:
      return Gfx::Vec2<double>(bounds.width()/2.0, -bounds.height()/2.0);
    case NE_ON_CENTER:
      return Gfx::Vec2<double>(-bounds.width()/2.0, -bounds.height()/2.0);
    case SW_ON_CENTER:
      return Gfx::Vec2<double>(bounds.width()/2.0, bounds.height()/2.0);
    case SE_ON_CENTER:
      return Gfx::Vec2<double>(-bounds.width()/2.0, bounds.height()/2.0);
    case ARBITRARY_ON_CENTER:
      return itsCenter;
    }
  // NATIVE_ALIGNMENT
  return bounds.center();
}

void GrObjAligner::doAlignment(Gfx::Canvas& canvas,
                               const Gfx::Rect<double>& native) const
{
DOTRACE("GrObjAligner::doAlignment");

  if (NATIVE_ALIGNMENT == itsMode) return;

  // This indicates where the center of the object will go
  Gfx::Vec2<double> center = getCenter(native);

  center -= native.center();

  Gfx::Vec3<double> vec(center.x(), center.y(), 0.0);

  canvas.translate(vec);
}


void GrObjAligner::draw(Gfx::Canvas& canvas) const
{
  Gfx::MatrixSaver state(canvas);

  doAlignment(canvas, child()->getBoundingBox(canvas));

  child()->draw(canvas);
}

void GrObjAligner::getBoundingCube(Gfx::Box<double>& cube,
                                   Gfx::Canvas& canvas) const
{
  Gfx::Rect<double> bounds = child()->getBoundingBox(canvas);

  Gfx::Vec2<double> center = getCenter(bounds);

  bounds.setCenter(center);

  cube.unionize(bounds);
}


static const char vcid_grobjaligner_cc[] = "$Header$";
#endif // !GROBJALIGNER_CC_DEFINED
