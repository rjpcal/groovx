/** @file gfx/linestrip.cc helper class for drawing line strips with
    optional corner joins  */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2004-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Thu Jun  3 17:03:05 2004
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [https://github.com/rjpcal/groovx]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#include "linestrip.h"

#include "geom/bezier4.h"
#include "geom/vec3.h"

#include "gfx/canvas.h"

#include "rutz/error.h"

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

using geom::vec2d;
using geom::vec3d;

Gfx::LineStrip::LineStrip() :
  canvas(nullptr),
  pts(0),
  width(1.0),
  join(false),
  loop(false)
{
GVX_TRACE("Gfx::LineStrip::LineStrip");
}

void Gfx::LineStrip::lineJoin(bool doJoin)
{
GVX_TRACE("Gfx::LineStrip::lineJoin");
  join = doJoin;
}

void Gfx::LineStrip::closeLoop(bool doClose)
{
GVX_TRACE("Gfx::LineStrip::closeLoop");
  loop = doClose;
}

void Gfx::LineStrip::begin(Gfx::Canvas& c, double w)
{
GVX_TRACE("Gfx::LineStrip::begin");

  if (pts.size() != 0)
    {
      throw rutz::error("LineStrip::end() not called before LineStrip::begin()", SRC_POS);
    }

  GVX_ASSERT(canvas == nullptr);

  canvas = &c;
  width = w;
}

void Gfx::LineStrip::vertex(const vec2d& pt)
{
GVX_TRACE("Gfx::LineStrip::vertex");
  pts.push_back(pt);
}

void Gfx::LineStrip::drawBezier4(const vec3d& p1,
                                 const vec3d& p2,
                                 const vec3d& p3,
                                 const vec3d& p4,
                                 unsigned int subdivisions,
                                 unsigned int start)
{
GVX_TRACE("Gfx::LineStrip::drawBezier4");
  geom::bezier4 xb(p1.x(), p2.x(), p3.x(), p4.x());
  geom::bezier4 yb(p1.y(), p2.y(), p3.y(), p4.y());
  //     geom::bezier4 zb(p1.z(), p2.z(), p3.z(), p4.z());

  for (unsigned int i = start; i < subdivisions; ++i)
    {
      double u = double(i) / double(subdivisions - 1);
      this->vertex(vec2d(xb.eval(u), yb.eval(u)));
    }
}

void Gfx::LineStrip::end()
{
GVX_TRACE("Gfx::LineStrip::end");

  if (pts.size() == 0) return;

  GVX_ASSERT(canvas != nullptr);

  if (join)
    drawJoinedLineStrip();
  else
    drawSimpleLineStrip();

  canvas = nullptr;
  pts.clear();
}

void Gfx::LineStrip::drawSimpleLineStrip()
{
GVX_TRACE("Gfx::LineStrip::drawSimpleLineStrip");
  if (loop)
    canvas->beginLineLoop();
  else
    canvas->beginLineStrip();
  for (unsigned int i = 0; i < pts.size(); ++i)
    {
      canvas->vertex2(pts[i]);
    }
  canvas->end();
}

namespace
{
  // Given two line segments with unit-normalized slopes of d1 and d2,
  // return the appropriate normal vector at the join point. The angle of
  // this result will be halfway between that of the normal vectors to d1
  // and d2, and the length of the result will be determined by the angle
  // of the intersection.
  vec2d joinedNormal(const vec2d& d1, const vec2d& d2)
  {
    const vec2d n1 = normal_to(d1);
    const vec2d n2 = normal_to(d2);

    double c = 0.0;
    if      (d1.x()+d2.x() != 0.0) { c = (n2.x() - n1.x()) / (d1.x() + d2.x()); }
    else if (d1.y()+d2.y() != 0.0) { c = (n2.y() - n1.y()) / (d1.y() + d2.y()); }

    return vec2d(n2 - d2 * c);
  }
}

void Gfx::LineStrip::drawJoinedLineStrip()
{
GVX_TRACE("Gfx::LineStrip::drawJoinedLineStrip");
  canvas->beginQuadStrip();

  vec2d d1;
  vec2d d2;

  vec2d start_pt1;
  vec2d start_pt2;

  for (unsigned int i = 1; i <= pts.size(); ++i)
    {
      if (i == pts.size())
        {
          if (loop) d2 = make_unit_length(pts[0] - pts[i-1]);
          else      d2 = d1;
        }
      else
        {
          d2 = make_unit_length(pts[i] - pts[i-1]);
        }

      if (i == 1)
        {
          if (loop) d1 = make_unit_length(pts[0] - pts.back());
          else      d1 = d2;
        }

      const vec2d use_n = joinedNormal(d1, d2);

      const vec2d pt1 = pts[i-1] - use_n*width;
      const vec2d pt2 = pts[i-1] + use_n*width;

      canvas->vertex2(pt1);
      canvas->vertex2(pt2);

      if (i == 1)
        {
          start_pt1 = pt1;
          start_pt2 = pt2;
        }

      d1 = d2;
    }

  if (loop)
    {
      canvas->vertex2(start_pt1);
      canvas->vertex2(start_pt2);
    }

  canvas->end();
}
