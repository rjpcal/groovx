///////////////////////////////////////////////////////////////////////
//
// gxaligner.cc
//
// Copyright (c) 2002-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Wed Nov 13 12:59:04 2002
// written: Wed Mar 19 12:46:04 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXALIGNER_CC_DEFINED
#define GXALIGNER_CC_DEFINED

#include "gxaligner.h"

#include "gfx/canvas.h"

#include "gx/bbox.h"
#include "gx/box.h"
#include "gx/rect.h"
#include "gx/txform.h"
#include "gx/vec3.h"

#include "util/trace.h"

GxAligner::GxAligner(Util::SoftRef<GxNode> child) :
  GxBin(child),
  itsMode(NATIVE_ALIGNMENT),
  itsCenter(0.0, 0.0)
{}

Gfx::Vec2<double> GxAligner::getCenter(const Gfx::Rect<double>& bounds) const
{
DOTRACE("GxAligner::getCenter");
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

void GxAligner::doAlignment(Gfx::Canvas& canvas,
                            const Gfx::Rect<double>& native) const
{
DOTRACE("GxAligner::doAlignment");

  if (NATIVE_ALIGNMENT == itsMode) return;

  // This indicates where the center of the object will go
  Gfx::Vec2<double> center = getCenter(native);

  center -= native.center();

  Gfx::Vec3<double> vec(center.x(), center.y(), 0.0);

  canvas.translate(vec);
}


void GxAligner::draw(Gfx::Canvas& canvas) const
{
DOTRACE("GxAligner::draw");
  Gfx::MatrixSaver state(canvas);

  doAlignment(canvas, child()->getBoundingBox(canvas));

  child()->draw(canvas);
}

void GxAligner::getBoundingCube(Gfx::Bbox& bbox) const
{
DOTRACE("GxAligner::getBoundingCube");

  Gfx::Bbox mybox = bbox.peer();

  child()->getBoundingCube(mybox);

  Gfx::Rect<double> bounds = mybox.rect();

  Gfx::Vec2<double> center = getCenter(bounds);

  bounds.setCenter(center);

  bbox.drawRect(bounds);

  dbgDump(2, bbox.cube());
}

static const char vcid_gxaligner_cc[] = "$Header$";
#endif // !GXALIGNER_CC_DEFINED
