///////////////////////////////////////////////////////////////////////
//
// canvas.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Nov 15 18:00:38 1999
// written: Fri Jan 18 16:07:07 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef CANVAS_CC_DEFINED
#define CANVAS_CC_DEFINED

#include "gfx/canvas.h"

#include "gx/vec3.h"

#include "util/arrays.h"

#include "util/trace.h"
#define LOCAL_DEBUG
#include "util/debug.h"

namespace
{
  struct BezData
  {
    Gfx::Vec3<double> pt0;
    Gfx::Vec3<double> pt1;
    Gfx::Vec3<double> pt2;
    Gfx::Vec3<double> pt3;
  };
}

Gfx::Canvas::~Canvas() {}

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

const char vcid_canvas_cc[] = "$Header$";
#endif // !CANVAS_CC_DEFINED
