///////////////////////////////////////////////////////////////////////
//
// grobjbbox.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Jul 19 10:45:53 2001
// written: Wed Nov 13 10:47:20 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GROBJBBOX_CC_DEFINED
#define GROBJBBOX_CC_DEFINED

#include "visx/grobjbbox.h"

#include "gfx/canvas.h"

#include "gx/rect.h"
#include "gx/rgbacolor.h"

#include "util/trace.h"
#include "util/debug.h"

namespace
{
  Gfx::Rect<double> addPixelBorder(const Gfx::Rect<double>& raw,
                                   int border_pixels)
  {

    Gfx::Rect<double> result = raw;

    result.widenByFactor(1.0 + border_pixels/100.0);
    result.heightenByFactor(1.0 + border_pixels/100.0);

    return result;
  }
}

GrObjBBox::GrObjBBox(Util::SoftRef<Gnode> child) :
  Gnode(child),
  isItVisible(false),
  itsPixelBorder(4),
  itsStipple(0x0F0F), // 0000111100001111
  itsMask(0x3333)     // 0011001100110011
{}

GrObjBBox::~GrObjBBox() {}

void GrObjBBox::gnodeBoundingBox(Gfx::Box<double>& cube,
                                 Gfx::Canvas& canvas) const
{
DOTRACE("GrObjBBox::gnodeBoundingBox");

  // Add extra pixels if the box itself will be visible.
  int border_pixels = isItVisible ? itsPixelBorder+4 : itsPixelBorder;

  dbgEval(3, itsPixelBorder); dbgEval(3, border_pixels);

  cube.unionize(addPixelBorder(child()->getBoundingBox(canvas),
                               border_pixels));
}

void GrObjBBox::draw(Gfx::Canvas& canvas) const
{
DOTRACE("GrObjBBox::draw");

  child()->draw(canvas);

  if (isItVisible)
    {
      Gfx::Rect<double> bounds =
        addPixelBorder(child()->getBoundingBox(canvas),
                       itsPixelBorder);

#define ANIMATE_BBOX

#ifdef ANIMATE_BBOX
      itsStipple ^= itsMask;

      itsMask = ~itsMask;
#endif

      Gfx::AttribSaver saver(canvas);

      canvas.setColor(Gfx::RgbaColor(0.0, 0.0, 1.0, 1.0));

      canvas.setLineWidth(1.0);
      canvas.setLineStipple(itsStipple);

      canvas.drawRect(bounds);

      canvas.setPointSize(4.0);

      {
        Gfx::PointsBlock block(canvas);

        canvas.vertex2(bounds.bottomLeft());
        canvas.vertex2(bounds.bottomRight());
        canvas.vertex2(bounds.topRight());
        canvas.vertex2(bounds.topLeft());
        canvas.vertex2(bounds.center());
      }
    }
}

static const char vcid_grobjbbox_cc[] = "$Header$";
#endif // !GROBJBBOX_CC_DEFINED
