///////////////////////////////////////////////////////////////////////
//
// gxbounds.cc
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Nov 13 13:34:18 2002
// written: Wed Nov 13 13:34:18 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXBOUNDS_CC_DEFINED
#define GXBOUNDS_CC_DEFINED

#include "gxbounds.h"

#include "gfx/canvas.h"

#include "gx/box.h"
#include "gx/rect.h"
#include "gx/rgbacolor.h"

#include "util/trace.h"
#include "util/debug.h"

GxBounds::GxBounds(Util::SoftRef<GxNode> child) :
  GxBin(child),
  isItVisible(false),
  itsPixelBorder(4),
  itsStipple(0x0F0F), // 0000111100001111
  itsMask(0x3333)     // 0011001100110011
{}

GxBounds::~GxBounds() {}

void GxBounds::getBoundingCube(Gfx::Box<double>& cube,
			       Gfx::Canvas& canvas) const
{
DOTRACE("GxBounds::getBoundingCube");

  child()->getBoundingCube(cube, canvas);

  int border_pixels = itsPixelBorder;

  // Add extra pixels if the box itself will be visible.
  if (isItVisible) border_pixels += 4;

  dbgEval(3, itsPixelBorder); dbgEvalNL(3, border_pixels);

  cube.scale(1.0 + border_pixels/100.0);
}

void GxBounds::draw(Gfx::Canvas& canvas) const
{
DOTRACE("GxBounds::draw");

  child()->draw(canvas);

  if (isItVisible)
    {
      Gfx::Box<double> cube;
      child()->getBoundingCube(cube, canvas);
      cube.scale(1.0 + itsPixelBorder/100.0);

      Gfx::Rect<double> bounds = cube.rect();

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

static const char vcid_gxbounds_cc[] = "$Header$";
#endif // !GXBOUNDS_CC_DEFINED
