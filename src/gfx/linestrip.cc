///////////////////////////////////////////////////////////////////////
//
// linestrip.cc
//
// Copyright (c) 2004-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Thu Jun  3 17:03:05 2004
// commit: $Id$
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

#ifndef LINESTRIP_CC_DEFINED
#define LINESTRIP_CC_DEFINED

#include "gfx/linestrip.h"

#include "gfx/canvas.h"

#include "gx/bezier4.h"
#include "gx/vec3.h"

#include "util/error.h"

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER

using Gfx::Vec2d;
using Gfx::Vec3d;

Gfx::LineStrip::LineStrip() :
  canvas(0),
  pts(0),
  width(1.0),
  join(false)
{
DOTRACE("Gfx::LineStrip::LineStrip");
}

void Gfx::LineStrip::lineJoin(bool doJoin)
{
DOTRACE("Gfx::LineStrip::lineJoin");
  join = doJoin;
}

void Gfx::LineStrip::begin(Gfx::Canvas& c, double w)
{
DOTRACE("Gfx::LineStrip::begin");

  if (pts.size() != 0)
    {
      throw Util::Error("LineStrip::end() not called before LineStrip::begin()");
    }

  Assert(canvas == 0);

  canvas = &c;
  width = w;
}

void Gfx::LineStrip::vertex(const Vec2d& pt)
{
DOTRACE("Gfx::LineStrip::vertex");
  pts.push_back(pt);
}

void Gfx::LineStrip::drawBezier4(const Vec3d& p1,
                                 const Vec3d& p2,
                                 const Vec3d& p3,
                                 const Vec3d& p4,
                                 unsigned int subdivisions,
                                 unsigned int start)
{
DOTRACE("Gfx::LineStrip::drawBezier4");
  Bezier4 xb(p1.x(), p2.x(), p3.x(), p4.x());
  Bezier4 yb(p1.y(), p2.y(), p3.y(), p4.y());
  //     Bezier4 zb(p1.z(), p2.z(), p3.z(), p4.z());

  for (unsigned int i = start; i < subdivisions; ++i)
    {
      double u = double(i) / double(subdivisions - 1);
      this->vertex(Vec2d(xb.eval(u), yb.eval(u)));
    }
}

void Gfx::LineStrip::end()
{
DOTRACE("Gfx::LineStrip::end");

  if (pts.size() == 0) return;

  Assert(canvas != 0);

  if (join)
    drawJoinedLineStrip();
  else
    drawSimpleLineStrip();

  canvas = 0;
  pts.clear();
}

void Gfx::LineStrip::drawSimpleLineStrip()
{
DOTRACE("Gfx::LineStrip::drawSimpleLineStrip");
  canvas->beginLineStrip();
  for (unsigned int i = 0; i < pts.size(); ++i)
    {
      canvas->vertex2(pts[i]);
    }
  canvas->end();
}

void Gfx::LineStrip::drawJoinedLineStrip()
{
DOTRACE("drawJoinedLineStrip");
  canvas->beginQuadStrip();

  Vec2d n1;
  Vec2d d1;

  for (unsigned int i = 1; i < pts.size(); ++i)
    {
      const Vec2d d2 = makeUnitLength(pts[i] - pts[i-1]);
      const Vec2d n2 = normalTo(d2);

      if (i == 1)
        {
          n1 = n2;
          d1 = d2;
        }

      double c = 0.0;
      if      (d1.x()+d2.x() != 0.0) { c = (n2.x() - n1.x()) / (d1.x() + d2.x()); }
      else if (d1.y()+d2.y() != 0.0) { c = (n2.y() - n1.y()) / (d1.y() + d2.y()); }

      const Vec2d use_n(n2 - d2 * c);

      canvas->vertex2(pts[i-1] - use_n*width);
      canvas->vertex2(pts[i-1] + use_n*width);

      n1 = n2;
      d1 = d2;
    }

  const Vec2d pt1(pts.back().x() - width*n1.x(),
                  pts.back().y() - width*n1.y());
  const Vec2d pt2(pts.back().x() + width*n1.x(),
                  pts.back().y() + width*n1.y());

  canvas->vertex2(pt1);
  canvas->vertex2(pt2);

  canvas->end();
}

static const char vcid_linestrip_cc[] = "$Header$";
#endif // !LINESTRIP_CC_DEFINED
