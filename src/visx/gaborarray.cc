///////////////////////////////////////////////////////////////////////
//
// gaborarray.cc
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon May 12 11:15:58 2003
// written: Tue May 13 10:07:19 2003
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

#ifndef GABORARRAY_CC_DEFINED
#define GABORARRAY_CC_DEFINED

#include "gaborarray.h"

#include "gx/geom.h"

#include "gx/bbox.h"
#include "gx/bmapdata.h"
#include "gx/imgfile.h"
#include "gx/rect.h"

#include "gfx/canvas.h"
#include "gfx/gxaligner.h"

#include "util/error.h"

#include "visx/gaborpatch.h"
#include "visx/snake.h"

#include <cstdio>

#include "util/debug.h"
#include "util/trace.h"

using namespace Gfx;

namespace
{
  const double SQRT3 = 1.73205080757;

  const int MAX_GABOR_NUMBER = 1800;
}

GaborArray::GaborArray(double gaborPeriod_, double gaborSigma_,
                       int foregNumber, double foregSpacing,
                       int sizeX, int sizeY,
                       double backgIniSpacing_,
                       double backgMinSpacing_)
  :
  itsForegNumber(foregNumber),
  itsForegSpacing(foregSpacing),
  itsSize(sizeX, sizeY),
  gaborPeriod(gaborPeriod_),
  gaborSigma(gaborSigma_),
  backgIniSpacing(backgIniSpacing_),
  backgMinSpacing(backgMinSpacing_),
  backgMinSpacingSqr(backgMinSpacing*backgMinSpacing),
  totalNumber(0),
  array(MAX_GABOR_NUMBER)
{
DOTRACE("GaborArray::GaborArray");

  setAlignmentMode(GxAligner::CENTER_ON_CENTER);

  rebuild();
}

void GaborArray::renderInto(BmapData& data) const
{
DOTRACE("GaborArray::renderInto");

  fixed_block<double> win(itsSize.x()*itsSize.y());

  for (int i = 0; i < itsSize.x()*itsSize.y(); ++i)
    win[i] = 0.0;

  for (int i = 0; i < totalNumber; ++i)
    {
      const double phi   = 2 * M_PI * drand48();
//       const double phi = 0.0;

      const double theta =
        (array[i].type == Element::CONTOUR)
        ? zerototwopi(array[i].theta + M_PI_2)
        : array[i].theta;

      const int xcenter = int(array[i].pos.x() + itsSize.x() / 2.0 + 0.5);
      const int ycenter = int(array[i].pos.y() + itsSize.y() / 2.0 + 0.5);

      const GaborPatch& p =
        GaborPatch::lookup(gaborSigma, 2*M_PI/gaborPeriod, theta, phi, 1.0);

      // bottom left:
      const int x0 = xcenter - p.size() / 2;
      const int y0 = ycenter - p.size() / 2;
      // top right:
      const int x1 = x0 + p.size();
      const int y1 = y0 + p.size();

      for (int y = y0; y < y1; ++y)
        for (int x = x0; x < x1; ++x)
          {
            if (x >= 0 && x < itsSize.x() && y >=0 && y < itsSize.y())
              win[x+y*itsSize.x()] += p.at(x-x0, y-y0);
          }
    }

  BmapData dest(itsSize, 8, 1);

  unsigned char* bytes = dest.bytesPtr();

  for (int k = 0; k < itsSize.x()*itsSize.y(); ++k)
    {
      const int val = int((win[k]+1.0)/2.0*255);
      Assert(val >= 0);
      Assert(val <= 255);
      *bytes++ = val;
    }

  data.swap(dest);
}

void GaborArray::saveImage(const char* filename) const
{
DOTRACE("GaborArray::saveImage");

  BmapData data;

  renderInto(data);

  ImgFile::save(filename, data);
}

void GaborArray::grGetBoundingBox(Bbox& bbox) const
{
DOTRACE("GaborArray::grGetBoundingBox");

  // FIXME this heavily duplicates Gabor::grGetBoundingBox()

  const Vec2d world_origin(0.0, 0.0);

  const Vec2i screen_origin = bbox.screenFromWorld(world_origin);

  Rect<int> screen_rect;
  screen_rect.setXYWH(screen_origin, itsSize);

  Rect<double> world_rect = bbox.worldFromScreen(screen_rect);

  bbox.drawRect(world_rect);
}

void GaborArray::grRender(Canvas& canvas) const
{
DOTRACE("GaborArray::grRender");

  BmapData data;

  renderInto(data);

  canvas.drawPixels(data, Vec2d(0.0, 0.0),
                    Vec2d(1.0, 1.0));
}

void GaborArray::rebuild()
{
DOTRACE("GaborArray::rebuild");

  Snake snake(itsForegNumber, itsForegSpacing);

  // pull in elements from the snake
  for (int n = 0; n < snake.getLength(); ++n)
    {
      if (!tryPush(snake.getElement(n)))
        {
          throw Util::Error("snake elements were too close together!\n");
        }
    }

  hexGridElements();

  const int diffusionCycles = 10;

  for (int i = 0; i < diffusionCycles; ++i)
    {
      jitterElement();
      fillElements();
    }

  const int insideNumber = insideElements(snake);

  printf(" FOREG_NUMBER %d    PATCH_NUMBER %d    TOTAL_NUMBER %d\n",
          snake.getLength(), insideNumber, totalNumber);
}

bool GaborArray::tryPush(const Element& e)
{
  if (tooClose(e.pos, -1)) return false;

  if (totalNumber >= MAX_GABOR_NUMBER)
    {
      throw Util::Error(fstring(" More than ", MAX_GABOR_NUMBER,
                                " elements!\n"));
    }

  array[totalNumber++] = e;

  dumpFrame();
  dumpFrame();

  return true;
}

bool GaborArray::tooClose(const Vec2d& v, int except)
{
  for (int n = 0; n < totalNumber; ++n)
    {
      const double dx = array[n].pos.x() - v.x();
      const double dy = array[n].pos.y() - v.y();

      if (dx*dx+dy*dy <= backgMinSpacingSqr && n != except)
        return true;
    }

  return false;
}

int GaborArray::insideElements(const Snake& snake)
{
DOTRACE("GaborArray::insideElements");

  int count = snake.getLength();

  for (int n = 0; n < totalNumber; ++n)
    {
      if (array[n].type == Element::CONTOUR)
        continue;

      bool inside = true;

      for (int i = 0; i < snake.getLength(); ++i)
        {
          const int j = (i+1) % snake.getLength();

          const double Yij = array[i].pos.x() - array[j].pos.x();
          const double Xij = array[j].pos.y() - array[i].pos.y();

          const double Xin = array[n].pos.x() - array[i].pos.x();
          const double Yin = array[n].pos.y() - array[i].pos.y();

          const double vp = Xij*Xin + Yij*Yin;

          if (vp < 0.0)
            {
              inside = false;
              continue;
            }
        }

      if (inside)
        {
          array[n].type = Element::INSIDE;
          ++count;
        }
    }

  return count;
}

void GaborArray::hexGridElements()
{
DOTRACE("GaborArray::hexGridElements");

  // lay down a hexagonal grid of elements

  const double dx = backgIniSpacing;
  const double dy = SQRT3 * backgIniSpacing / 2.0;

  const int nx = int((itsSize.x() - backgMinSpacing) / dx - 0.5);
  const int ny = int((itsSize.y() - backgMinSpacing) / dy);

  double y = -0.5 * (ny-1) * dy;

  for (int j = 0; j < ny; ++j, y += dy)
    {
      double x = -0.5 * (nx-1) * dx - 0.25 * dx;

      // this is a hexagonal grid, so every other row is staggered by half
      // a step in the x direction
      if (j%2) x += 0.5*dx;

      for (int i = 0; i < nx; ++i, x += dx)
        {
          tryPush(Element(x, y, 2 * M_PI * drand48(), Element::OUTSIDE));
        }
    }
}

void GaborArray::fillElements()
{
DOTRACE("GaborArray::fillElements");

  const double tryFillArea = 6.0;

  const double dx = sqrt(tryFillArea);

  const double halfX = 0.5 * itsSize.x();
  const double halfY = 0.5 * itsSize.y();

  for (double x = -halfX; x <= halfX; x += dx)
    for (double y = -halfY; y <= halfY; y += dx)
      {
        tryPush(Element(x, y, 2 * M_PI * drand48(), Element::OUTSIDE));
      }

  const double backgAveSpacing = sqrt(2.0*itsSize.x()*itsSize.y()/(SQRT3*totalNumber));
  printf(" %d elements, ave spacing %f\n", totalNumber, backgAveSpacing);
}

void GaborArray::jitterElement()
{
DOTRACE("GaborArray::jitterElement");

  const double jitter = (backgMinSpacing/16.0);

  const int backgroundIters = 1000;

  const double halfX = 0.5 * itsSize.x();
  const double halfY = 0.5 * itsSize.y();

  for (int niter = 0; niter < backgroundIters; ++niter)
    {
      for (int n = 0; n < totalNumber; ++n)
        {
          if (array[n].type == Element::CONTOUR)
            continue;

          Vec2d v;
          v.x() = array[n].pos.x() + jitter*(2*drand48() - 1);
          v.y() = array[n].pos.y() + jitter*(2*drand48() - 1);

          if (v.x() < -halfX) v.x() += itsSize.x();
          if (v.x() >  halfX) v.x() -= itsSize.x();
          if (v.y() < -halfY) v.y() += itsSize.y();
          if (v.y() >  halfY) v.y() -= itsSize.y();

          if (!tooClose(v, n))
            {
              array[n].pos = v;
            }
        }

      if (niter % 15 == 0) dumpFrame();
    }
}

void GaborArray::dumpFrame() const
{
DOTRACE("GaborArray::dumpFrame");

  if (0)
    {
      static int framecount = 0;

      char fname[256];
      snprintf(fname, 256, "frame_%06d.pnm", framecount++);

      this->saveImage(fname);

      printf("dumped frame %s\n", fname);
    }
}

static const char vcid_gaborarray_cc[] = "$Header$";
#endif // !GABORARRAY_CC_DEFINED
