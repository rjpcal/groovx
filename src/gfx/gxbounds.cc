///////////////////////////////////////////////////////////////////////
//
// gxbounds.cc
//
// Copyright (c) 2002-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Wed Nov 13 13:34:18 2002
// written: Wed Mar 19 17:56:05 2003
// $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef GXBOUNDS_CC_DEFINED
#define GXBOUNDS_CC_DEFINED

#include "gxbounds.h"

#include "gfx/canvas.h"

#include "gx/bbox.h"
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

void GxBounds::getBoundingCube(Gfx::Bbox& bbox) const
{
DOTRACE("GxBounds::getBoundingCube");

  int border_pixels = itsPixelBorder;

  dbgEval(3, itsPixelBorder); dbgEvalNL(3, border_pixels);

  bbox.push();

  const double s = 1.0 + border_pixels/100.0;

  bbox.scale(Gfx::Vec3<double>(s,s,s));

  child()->getBoundingCube(bbox);

  bbox.pop();

  dbgDump(2, bbox.cube());
}

void GxBounds::draw(Gfx::Canvas& canvas) const
{
DOTRACE("GxBounds::draw");

  child()->draw(canvas);

  if (isItVisible)
    {
      Gfx::Bbox bbox(canvas);
      const double s = 1.0 + itsPixelBorder/100.0;
      bbox.scale(Gfx::Vec3<double>(s,s,s));
      child()->getBoundingCube(bbox);

      Gfx::Rect<double> bounds = bbox.rect();

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
