///////////////////////////////////////////////////////////////////////
//
// canvas.cc
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Mon Nov 15 18:00:38 1999
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

#ifndef CANVAS_CC_DEFINED
#define CANVAS_CC_DEFINED

#include "gfx/canvas.h"

#include "gx/bezier4.h"
#include "gx/box.h"
#include "gx/rgbacolor.h"
#include "gx/vec3.h"

#include "util/arrays.h"
#include "util/error.h"

#include "util/debug.h"
#include "util/trace.h"

namespace
{
  struct BezData
  {
    Gfx::Vec3<double> pt0;
    Gfx::Vec3<double> pt1;
    Gfx::Vec3<double> pt2;
    Gfx::Vec3<double> pt3;
  };

  Gfx::Canvas* appCanvas = 0;
}

void Gfx::Canvas::setCurrent(Gfx::Canvas& canvas)
{
DOTRACE("Gfx::Canvas::setCurrent");

  Assert(&canvas != 0);

  appCanvas = &canvas;
}

Gfx::Canvas& Gfx::Canvas::current()
{
DOTRACE("Gfx::Canvas::current");

  if (appCanvas == 0)
    throw Util::Error("appCanvas not inited");

  return *appCanvas;
}

Gfx::Canvas::~Canvas() throw() {}

void Gfx::Canvas::drawBox(const Gfx::Box<double>& box)
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

void Gfx::Canvas::drawBezier4(const Gfx::Vec3<double>& p1,
                              const Gfx::Vec3<double>& p2,
                              const Gfx::Vec3<double>& p3,
                              const Gfx::Vec3<double>& p4,
                              unsigned int subdivisions)
{
DOTRACE("Gfx::Canvas::drawBezier4");

  Bezier4 xb(p1.x(), p2.x(), p3.x(), p4.x());
  Bezier4 yb(p1.y(), p2.y(), p3.y(), p4.y());
  Bezier4 zb(p1.z(), p2.z(), p3.z(), p4.z());

  beginLineStrip();
  for (unsigned int i = 0; i < subdivisions; ++i)
    {
      double u = double(i) / double(subdivisions - 1);
      vertex3(Gfx::Vec3<double>(xb.eval(u), yb.eval(u), zb.eval(u)));
    }
  end();
}

void Gfx::Canvas::drawBezierFill4(const Gfx::Vec3<double>& center,
                                  const Gfx::Vec3<double>& p1,
                                  const Gfx::Vec3<double>& p2,
                                  const Gfx::Vec3<double>& p3,
                                  const Gfx::Vec3<double>& p4,
                                  unsigned int subdivisions)
{
DOTRACE("Gfx::Canvas::drawBezierFill4");

  Bezier4 xb(p1.x(), p2.x(), p3.x(), p4.x());
  Bezier4 yb(p1.y(), p2.y(), p3.y(), p4.y());
  Bezier4 zb(p1.z(), p2.z(), p3.z(), p4.z());

  beginTriangleFan();
  vertex3(center);
  for (unsigned int i = 0; i < subdivisions; ++i)
    {
      double u = double(i) / double(subdivisions - 1);
      vertex3(Gfx::Vec3<double>(xb.eval(u), yb.eval(u), zb.eval(u)));
    }
  end();
}

void Gfx::Canvas::drawNurbsCurve(const dynamic_block<float>& knots,
                                 const dynamic_block<Gfx::Vec3<float> >& pts)
{
DOTRACE("Gfx::Canvas::drawNurbsCurve");

  const float* t = &knots[2];
  // t points to { 0, 0, 0.17, 0.33, 0.5, 0.67, 0.83, 1, 1 }

  unsigned int nctrl = pts.size();

  Assert(nctrl > 4);

  unsigned int nbz = nctrl - 3;

  dynamic_block<BezData> bz(nbz);

  for (unsigned int k = 0; k < nbz; ++k)
    {
      float d1 = t[k+3] - t[k+2]; // == 0 when k == nbz-1 (last iteration)
      float d2 = t[k+2] - t[k+1];
      float d3 = t[k+1] - t[k];  // == 0 when k == 0 (first iteration)
      float d = t[k+3] - t[k];

      bz[k].pt1 = (pts[k+2] *  d3     + pts[k+1] * (d1+d2)) / d;
      bz[k].pt2 = (pts[k+2] * (d2+d3) + pts[k+1] *  d1    ) / d;

      if (k == 0)
        {
          bz[k].pt0 = pts[k];
        }
      else
        {
          bz[k-1].pt3 = (bz[k-1].pt2 * d2 + bz[k].pt1 * d3) / (d2+d3);
          bz[k].pt0 = bz[k-1].pt3;
        }

      if (k == (nbz-1))
        {
          bz[k].pt3 = pts[k+3];
        }

    }

  for (unsigned int i = 0; i < bz.size(); ++i)
    {
      drawBezier4(bz[i].pt0, bz[i].pt1, bz[i].pt2, bz[i].pt3, 20);
    }
}

void Gfx::Canvas::begin(Gfx::Canvas::VertexStyle s)
{
  switch (s)
    {
    case POINTS:         beginPoints(); break;
    case LINES:          beginLines(); break;
    case LINE_STRIP:     beginLineStrip(); break;
    case LINE_LOOP:      beginLineLoop(); break;
    case TRIANGLES:      beginTriangles(); break;
    case TRIANGLE_STRIP: beginTriangleStrip(); break;
    case TRIANGLE_FAN:   beginTriangleFan(); break;
    case QUADS:          beginQuads(); break;
    case QUAD_STRIP:     beginQuadStrip(); break;
    case POLYGON:        beginPolygon(); break;
    }
}

void Gfx::Canvas::finishDrawing()
{
DOTRACE("Gfx::Canvas::finishDrawing");
}

const char vcid_canvas_cc[] = "$Header$";
#endif // !CANVAS_CC_DEFINED
