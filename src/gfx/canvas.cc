///////////////////////////////////////////////////////////////////////
//
// canvas.cc
//
// Copyright (c) 1999-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Mon Nov 15 18:00:38 1999
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
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

#ifndef CANVAS_CC_DEFINED
#define CANVAS_CC_DEFINED

#include "canvas.h"

#include "gfx/rgbacolor.h"

#include "geom/bezier4.h"
#include "geom/box.h"
#include "geom/vec3.h"

#include "util/arrays.h"
#include "util/error.h"

#include "util/debug.h"
DBG_REGISTER
#include "util/trace.h"

using geom::recti;
using geom::rectd;
using geom::vec2i;
using geom::vec2d;
using geom::vec3i;
using geom::vec3d;

namespace
{
  struct BezData
  {
    vec3d pt0;
    vec3d pt1;
    vec3d pt2;
    vec3d pt3;
  };
}

Gfx::Canvas::~Canvas() throw() {}

vec2d Gfx::Canvas::screenFromWorld2(const vec2d& world_pos) const
{
DOTRACE("Gfx::Canvas::screenFromWorld2");

  return screenFromWorld3(vec3d(world_pos.x(),
                                world_pos.y(),
                                0.0)).as_vec2();
}

recti Gfx::Canvas::screenBoundsFromWorldRect(const rectd& world_pos) const
{
DOTRACE("Gfx::Canvas::screenBoundsFromWorldRect");

  // In order to get an accurate bounding box in screen coords, we need
  // to consider all four corners in world coordinates.
  const vec2i p1(screenFromWorld2(world_pos.bottom_left()));
  const vec2i p2(screenFromWorld2(world_pos.top_right()));
  const vec2i p3(screenFromWorld2(world_pos.bottom_right()));
  const vec2i p4(screenFromWorld2(world_pos.top_left()));

  const recti screen_rect1(p1, p2);
  const recti screen_rect2(p3, p4);

  return screen_rect1.union_with(screen_rect2);
}

void Gfx::Canvas::drawRect(const rectd& rect, bool filled)
{
  Gfx::AttribSaver saver(*this);

  this->setPolygonFill(filled);

  drawRect(rect);
}

void Gfx::Canvas::drawBox(const geom::box<double>& box)
{
  AttribSaver saver(*this);

  {
    setColor(RgbaColor(1.0, 0.0, 0.0, 1.0));
    LineStripBlock block(*this);
    vertex3(box.point000());
    vertex3(box.point100());
    vertex3(box.point110());
    vertex3(box.point111());
  }

  {
    setColor(RgbaColor(0.0, 1.0, 0.0, 1.0));
    LineStripBlock block(*this);
    vertex3(box.point001());
    vertex3(box.point000());
    vertex3(box.point010());
    vertex3(box.point110());
  }

  {
    setColor(RgbaColor(0.0, 0.0, 1.0, 1.0));
    LineStripBlock block(*this);
    vertex3(box.point101());
    vertex3(box.point001());
    vertex3(box.point011());
    vertex3(box.point010());
  }

  {
    setColor(RgbaColor(0.0, 0.0, 0.0, 1.0));
    LineStripBlock block(*this);
    vertex3(box.point011());
    vertex3(box.point111());
    vertex3(box.point101());
    vertex3(box.point100());
  }
}

void Gfx::Canvas::drawBezier4(const vec3d& p1,
                              const vec3d& p2,
                              const vec3d& p3,
                              const vec3d& p4,
                              unsigned int subdivisions)
{
DOTRACE("Gfx::Canvas::drawBezier4");

  geom::bezier4 xb(p1.x(), p2.x(), p3.x(), p4.x());
  geom::bezier4 yb(p1.y(), p2.y(), p3.y(), p4.y());
  geom::bezier4 zb(p1.z(), p2.z(), p3.z(), p4.z());

  beginLineStrip();
  for (unsigned int i = 0; i < subdivisions; ++i)
    {
      double u = double(i) / double(subdivisions - 1);
      vertex3(vec3d(xb.eval(u), yb.eval(u), zb.eval(u)));
    }
  end();
}

void Gfx::Canvas::drawBezierFill4(const vec3d& center,
                                  const vec3d& p1,
                                  const vec3d& p2,
                                  const vec3d& p3,
                                  const vec3d& p4,
                                  unsigned int subdivisions)
{
DOTRACE("Gfx::Canvas::drawBezierFill4");

  geom::bezier4 xb(p1.x(), p2.x(), p3.x(), p4.x());
  geom::bezier4 yb(p1.y(), p2.y(), p3.y(), p4.y());
  geom::bezier4 zb(p1.z(), p2.z(), p3.z(), p4.z());

  beginTriangleFan();
  vertex3(center);
  for (unsigned int i = 0; i < subdivisions; ++i)
    {
      double u = double(i) / double(subdivisions - 1);
      vertex3(vec3d(xb.eval(u), yb.eval(u), zb.eval(u)));
    }
  end();
}

void Gfx::Canvas::drawNurbsCurve
  (const rutz::dynamic_block<float>& knots,
   const rutz::dynamic_block<geom::vec3<float> >& pts)
{
DOTRACE("Gfx::Canvas::drawNurbsCurve");

  const float* t = &knots[2];
  // t points to { 0, 0, 0.17, 0.33, 0.5, 0.67, 0.83, 1, 1 }

  unsigned int nctrl = pts.size();

  ASSERT(nctrl > 4);

  unsigned int nbz = nctrl - 3;

  rutz::dynamic_block<BezData> bz(nbz);

  for (unsigned int k = 0; k < nbz; ++k)
    {
      float d1 = t[k+3] - t[k+2]; // == 0 when k == nbz-1 (last iteration)
      float d2 = t[k+2] - t[k+1];
      float d3 = t[k+1] - t[k];  // == 0 when k == 0 (first iteration)
      float d = t[k+3] - t[k];

      bz[k].pt1 = vec3d((pts[k+2] *  d3     + pts[k+1] * (d1+d2)) / d);
      bz[k].pt2 = vec3d((pts[k+2] * (d2+d3) + pts[k+1] *  d1    ) / d);

      if (k == 0)
        {
          bz[k].pt0 = vec3d(pts[k]);
        }
      else
        {
          bz[k-1].pt3 = (bz[k-1].pt2 * d2 + bz[k].pt1 * d3) / (d2+d3);
          bz[k].pt0 = bz[k-1].pt3;
        }

      if (k == (nbz-1))
        {
          bz[k].pt3 = vec3d(pts[k+3]);
        }

    }

  for (unsigned int i = 0; i < bz.size(); ++i)
    {
      drawBezier4(bz[i].pt0, bz[i].pt1, bz[i].pt2, bz[i].pt3, 20);
    }
}

void Gfx::Canvas::begin(Gfx::Canvas::VertexStyle s, const char* comment)
{
  switch (s)
    {
    case POINTS:         beginPoints(comment); break;
    case LINES:          beginLines(comment); break;
    case LINE_STRIP:     beginLineStrip(comment); break;
    case LINE_LOOP:      beginLineLoop(comment); break;
    case TRIANGLES:      beginTriangles(comment); break;
    case TRIANGLE_STRIP: beginTriangleStrip(comment); break;
    case TRIANGLE_FAN:   beginTriangleFan(comment); break;
    case QUADS:          beginQuads(comment); break;
    case QUAD_STRIP:     beginQuadStrip(comment); break;
    case POLYGON:        beginPolygon(comment); break;
    }
}

void Gfx::Canvas::finishDrawing()
{
DOTRACE("Gfx::Canvas::finishDrawing");
}

const char vcid_canvas_cc[] = "$Header$";
#endif // !CANVAS_CC_DEFINED
