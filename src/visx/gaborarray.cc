///////////////////////////////////////////////////////////////////////
//
// gaborarray.cc
//
// Copyright (c) 2002-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Mon May 12 11:15:58 2003
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

#ifndef GABORARRAY_CC_DEFINED
#define GABORARRAY_CC_DEFINED

#include "gaborarray.h"

#include "gfx/canvas.h"
#include "gfx/gxaligner.h"

#include "gx/bbox.h"
#include "gx/bmapdata.h"
#include "gx/geom.h"
#include "gx/imgfile.h"
#include "gx/rect.h"

#include "io/ioproxy.h"
#include "io/reader.h"
#include "io/writer.h"

#include "util/error.h"
#include "util/rand.h"

#include "visx/gaborpatch.h"
#include "visx/snake.h"

#include <cstdio>

#include "util/debug.h"
DBG_REGISTER;
#include "util/trace.h"

using namespace Geom;
using namespace Gfx;

namespace
{
  const int MAX_GABOR_NUMBER = 1800;
}

GaborArray::GaborArray(double gaborPeriod, double gaborSigma,
                       int foregNumber, double foregSpacing,
                       int sizeX, int sizeY,
                       double gridSpacing,
                       double minSpacing)
  :
  itsForegSeed(0),
  itsForegNumber(foregNumber),
  itsForegSpacing(foregSpacing),
  itsForegPosX(0),
  itsForegPosY(0),

  itsBackgSeed(0),
  itsSizeX(sizeX),
  itsSizeY(sizeY),
  itsGridSpacing(gridSpacing),
  itsMinSpacing(minSpacing),
  itsFillResolution(15.0),

  itsThetaSeed(0),
  itsPhaseSeed(0),
  itsThetaJitter(0.0),
  itsGaborPeriod(gaborPeriod),
  itsGaborSigma(gaborSigma),
  itsContrastSeed(0),
  itsContrastJitter(0.0),

  itsTotalNumber(0),
  itsArray(MAX_GABOR_NUMBER),
  itsBmap(0)
{
DOTRACE("GaborArray::GaborArray");

  setAlignmentMode(GxAligner::CENTER_ON_CENTER);
  setPercentBorder(0);
}

GaborArray::~GaborArray() throw() {}

const FieldMap& GaborArray::classFields()
{
  static const Field FIELD_ARRAY[] =
  {
    Field("foregSeed", &GaborArray::itsForegSeed, 0, 0, 20000, 1,
          Field::NEW_GROUP),
    Field("foregNumber", &GaborArray::itsForegNumber, 24, 1, 100, 1),
    Field("foregSpacing", &GaborArray::itsForegSpacing,
          45.0, 1.0, 100.0, 1.0),
    Field("foregPosX", &GaborArray::itsForegPosX, 0, -2048, 2048, 1),
    Field("foregPosY", &GaborArray::itsForegPosY, 0, -2048, 2048, 1),

    Field("backgSeed", &GaborArray::itsBackgSeed, 0, 0, 20000, 1),
    Field("sizeX", &GaborArray::itsSizeX, 512, 16, 2048, 16),
    Field("sizeY", &GaborArray::itsSizeY, 512, 16, 2048, 16),
    Field("gridSpacing", &GaborArray::itsGridSpacing, 48., 1., 200., 1.),
    Field("minSpacing", &GaborArray::itsMinSpacing, 36., 1., 200., 1.),
    Field("fillResolution", &GaborArray::itsFillResolution, 15., 1., 50., 1.),

    Field("thetaSeed", &GaborArray::itsThetaSeed, 0, 0, 20000, 1),
    Field("phaseSeed", &GaborArray::itsPhaseSeed, 0, 0, 20000, 1),
    Field("thetaJitter", &GaborArray::itsThetaJitter, 0.0, 0.0, 1.0, 0.01),
    Field("gaborPeriod", &GaborArray::itsGaborPeriod, 15.0, 1.0, 50.0, 1.0),
    Field("gaborSigma", &GaborArray::itsGaborSigma, 7.5, 0.5, 25.0, 0.5),
    Field("contrastSeed", &GaborArray::itsContrastSeed, 0, 0, 20000, 1),
    Field("contrastJitter", &GaborArray::itsContrastJitter, 0.0, 0.0, 2.0, 0.01),
  };

  static FieldMap GABORARRAY_FIELDS(FIELD_ARRAY, &GxShapeKit::classFields());

  return GABORARRAY_FIELDS;
}

void GaborArray::readFrom(IO::Reader& reader)
{
DOTRACE("GaborArray::readFrom");

  readFieldsFrom(reader, classFields());

  reader.readBaseClass("GxShapeKit", IO::makeProxy<GxShapeKit>(this));
}

void GaborArray::writeTo(IO::Writer& writer) const
{
DOTRACE("GaborArray::writeTo");

  writeFieldsTo(writer, classFields());

  writer.writeBaseClass("GxShapeKit", IO::makeConstProxy<GxShapeKit>(this));
}

void GaborArray::saveImage(const char* filename) const
{
DOTRACE("GaborArray::saveImage");

  update();

  ImgFile::save(filename, *itsBmap);
}

void GaborArray::saveContourOnlyImage(const char* filename) const
{
DOTRACE("GaborArray::saveContourOnlyImage");

  updateForeg();

  const int npix = itsSizeX*itsSizeY;

  fixed_block<double> win(npix);

  for (int i = 0; i < npix; ++i)
    win[i] = 0.0;

  for (int i = 0; i < itsTotalNumber; ++i)
    {
      if (itsArray[i].type != Element::CONTOUR)
        continue;

      const double theta = rad_0_2pi(itsArray[i].theta + M_PI_2);

      const int xcenter = int(itsArray[i].pos.x() + itsSizeX / 2.0 + 0.5);
      const int ycenter = int(itsArray[i].pos.y() + itsSizeY / 2.0 + 0.5);

      const double period = 1.5*itsForegSpacing;

      const GaborPatch& p =
        GaborPatch::lookup(itsForegSpacing/2.0, 2*M_PI/period,
                           theta, 0.0);

      // bottom left:
      const int x0 = xcenter - p.size() / 2;
      const int y0 = ycenter - p.size() / 2;
      // top right:
      const int x1 = x0 + p.size();
      const int y1 = y0 + p.size();

      for (int y = y0; y < y1; ++y)
        for (int x = x0; x < x1; ++x)
          {
            if (x >= 0 && x < itsSizeX && y >=0 && y < itsSizeY)
              win[x+y*itsSizeX] += p.at(x-x0, y-y0);
          }
    }

  shared_ptr<BmapData> result(new BmapData(Vec2i(itsSizeX, itsSizeY),
                                           8, 1));

  unsigned char* bytes = result->bytesPtr();

  for (int k = 0; k < npix; ++k)
    {
      int val = int(win[k]*255);

      // Threshold anything below 128 here, so that we don't get "ghosts"
      // showing up from the second peaks of the gabor functions.
      if      (val < 128) { val = 0; }
      else if (val > 255) { val = 255; }

      *bytes++ = val;
    }

  ImgFile::save(filename, *result);
}

void GaborArray::grGetBoundingBox(Bbox& bbox) const
{
DOTRACE("GaborArray::grGetBoundingBox");

  // FIXME this heavily duplicates Gabor::grGetBoundingBox()

  const Vec2d world_origin(0.0, 0.0);

  const Vec2i screen_origin = bbox.screenFromWorld(world_origin);

  Rect<int> screen_rect;
  screen_rect.setXYWH(screen_origin, Vec2i(itsSizeX, itsSizeY));

  Rect<double> world_rect = bbox.worldFromScreen(screen_rect);

  bbox.drawRect(world_rect);
}

void GaborArray::grRender(Canvas& canvas) const
{
DOTRACE("GaborArray::grRender");

  update();

  canvas.drawPixels(*itsBmap, Vec2d(0.0, 0.0), Vec2d(1.0, 1.0));
}

void GaborArray::updateForeg() const
{
DOTRACE("GaborArray::updateForeg");

  if (itsForegSeed.ok()
      && itsForegNumber.ok()
      && itsForegSpacing.ok()
      && itsForegPosX.ok()
      && itsForegPosY.ok())
    return;

  itsTotalNumber = 0;

  Util::Urand urand(itsForegSeed);

  Snake snake(itsForegNumber, itsForegSpacing, urand);

  // pull in elements from the snake
  for (int n = 0; n < itsForegNumber; ++n)
    {
      Element e = snake.getElement(n);
      e.pos.x() += itsForegPosX;
      e.pos.y() += itsForegPosY;

      if (!tryPush(e))
        {
          throw Util::Error("foreground elements were too close together!\n");
        }
    }

  itsForegSeed.save();
  itsForegNumber.save();
  itsForegSpacing.save();
  itsForegPosX.save();
  itsForegPosY.save();

  itsBackgSeed.touch(); // to force a redo in updateBackg()

  Assert(itsTotalNumber == itsForegNumber);
}

void GaborArray::updateBackg() const
{
DOTRACE("GaborArray::updateBackg");

  if (itsBackgSeed.ok()
      && itsSizeX.ok()
      && itsSizeY.ok()
      && itsGridSpacing.ok()
      && itsMinSpacing.ok()
      && itsFillResolution.ok())
    return;

  itsTotalNumber = itsForegNumber;

  backgHexGrid();

  const int diffusionCycles = 10;

  Util::Urand urand(itsBackgSeed);

  for (int i = 0; i < diffusionCycles; ++i)
    {
      backgJitter(urand);
      backgFill();
    }

  int insideNumber = itsForegNumber;

  for (int n = 0; n < itsTotalNumber; ++n)
    {
      if (itsArray[n].type == Element::CONTOUR)
        continue;

      bool inside = true;

      for (int i = 0; i < itsForegNumber; ++i)
        {
          const int j = (i+1) % itsForegNumber;

          // This is the vector perpendicular to the line connecting
          // contour nodes i and j
          const double Yij = itsArray[i].pos.x() - itsArray[j].pos.x();
          const double Xij = itsArray[j].pos.y() - itsArray[i].pos.y();

          // This is the vector connecting from contour node i to the
          // background node
          const double Xin = itsArray[n].pos.x() - itsArray[i].pos.x();
          const double Yin = itsArray[n].pos.y() - itsArray[i].pos.y();

          // If the dot product of those two vectors is less than zero,
          // then the background node is "outside".
          const double vp = Xij*Xin + Yij*Yin;

          if (vp < 0.0) { inside = false; break; }
        }

      if (inside)
        {
          itsArray[n].type = Element::INSIDE;
          ++insideNumber;
        }
    }

  printf(" FOREG_NUMBER %d    PATCH_NUMBER %d    TOTAL_NUMBER %d\n",
         itsForegNumber.val, insideNumber, itsTotalNumber);

  itsBackgSeed.save();
  itsSizeX.save();
  itsSizeY.save();
  itsGridSpacing.save();
  itsMinSpacing.save();
  itsFillResolution.save();

  itsThetaSeed.touch(); // to force a redo in updateBmap()
}

void GaborArray::updateBmap() const
{
DOTRACE("GaborArray::updateBmap");

  if (itsThetaSeed.ok()
      && itsPhaseSeed.ok()
      && itsThetaJitter.ok()
      && itsGaborPeriod.ok()
      && itsGaborSigma.ok()
      && itsContrastSeed.ok()
      && itsContrastJitter.ok())
    return;

  const int npix = itsSizeX*itsSizeY;

  fixed_block<double> win(npix);

  for (int i = 0; i < npix; ++i)
    win[i] = 0.0;

  Util::Urand thetas(itsThetaSeed);
  Util::Urand phases(itsPhaseSeed);
  Util::Urand contrasts(itsContrastSeed);

  for (int i = 0; i < itsTotalNumber; ++i)
    {
      const double phi   = 2 * M_PI * phases.fdraw();
//       const double phi = 0.0;

      const double rand_theta = 2*M_PI * thetas.fdraw();

      const double theta =
        (itsArray[i].type == Element::CONTOUR)
        ? rad_0_2pi(itsThetaJitter * (rand_theta - M_PI) +
                    (itsArray[i].theta + M_PI_2))
        : rand_theta;

      const int xcenter = int(itsArray[i].pos.x() + itsSizeX / 2.0 + 0.5);
      const int ycenter = int(itsArray[i].pos.y() + itsSizeY / 2.0 + 0.5);

      const GaborPatch& p =
        GaborPatch::lookup(itsGaborSigma, 2*M_PI/itsGaborPeriod,
                           theta, phi);

      // bottom left:
      const int x0 = xcenter - p.size() / 2;
      const int y0 = ycenter - p.size() / 2;
      // top right:
      const int x1 = x0 + p.size();
      const int y1 = y0 + p.size();

      const double contrast = exp(-itsContrastJitter * contrasts.fdraw());

      for (int y = y0; y < y1; ++y)
        for (int x = x0; x < x1; ++x)
          {
            if (x >= 0 && x < itsSizeX && y >=0 && y < itsSizeY)
              win[x+y*itsSizeX] += contrast * p.at(x-x0, y-y0);
          }
    }

  shared_ptr<BmapData> result(new BmapData(Vec2i(itsSizeX, itsSizeY),
                                           8, 1));

  unsigned char* bytes = result->bytesPtr();

  bool clip = false;

  for (int k = 0; k < npix; ++k)
    {
      int val = int((win[k]+1.0)/2.0*255);

      if      (val < 0)   { clip = true; val = 0; }
      else if (val > 255) { clip = true; val = 255; }

      Assert(val >= 0);
      Assert(val <= 255);

      *bytes++ = val;
    }

  if (clip)
    printf("warning: some values were clipped\n");

  itsBmap.swap(result);

  itsThetaSeed.save();
  itsPhaseSeed.save();
  itsThetaJitter.save();
  itsGaborPeriod.save();
  itsGaborSigma.save();
  itsContrastSeed.save();
  itsContrastJitter.save();
}

void GaborArray::update() const
{
DOTRACE("GaborArray::update");

  updateForeg();

  updateBackg();

  updateBmap();
}

bool GaborArray::tryPush(const Element& e) const
{
  if (tooClose(e.pos, -1)) return false;

  if (itsTotalNumber >= MAX_GABOR_NUMBER)
    {
      throw Util::Error(fstring(" More than ", MAX_GABOR_NUMBER,
                                " elements!\n"));
    }

  itsArray[itsTotalNumber++] = e;

  dumpFrame();
  dumpFrame();

  return true;
}

bool GaborArray::tooClose(const Vec2d& v, int except) const
{
  double minSpacingSqr = itsMinSpacing*itsMinSpacing;

  for (int n = 0; n < itsTotalNumber; ++n)
    {
      const double dx = itsArray[n].pos.x() - v.x();
      const double dy = itsArray[n].pos.y() - v.y();

      if (dx*dx+dy*dy <= minSpacingSqr && n != except)
        return true;
    }

  return false;
}

void GaborArray::backgHexGrid() const
{
DOTRACE("GaborArray::backgHexGrid");

  // lay down a hexagonal grid of elements

  const double dx = itsGridSpacing;
  const double dy = sqrt(3.0) * itsGridSpacing / 2.0;

  const int nx = int((itsSizeX - itsMinSpacing) / dx - 0.5);
  const int ny = int((itsSizeY - itsMinSpacing) / dy);

  double y = -0.5 * (ny-1) * dy;

  for (int j = 0; j < ny; ++j, y += dy)
    {
      double x = -0.5 * (nx-1) * dx - 0.25 * dx;

      // this is a hexagonal grid, so every other row is staggered by half
      // a step in the x direction
      if (j%2) x += 0.5*dx;

      for (int i = 0; i < nx; ++i, x += dx)
        {
          tryPush(Element(x, y, 0.0, Element::OUTSIDE));
        }
    }
}

void GaborArray::backgFill() const
{
DOTRACE("GaborArray::backgFill");

  const double dx = itsMinSpacing / itsFillResolution;

  const double halfX = 0.5 * itsSizeX;
  const double halfY = 0.5 * itsSizeY;

  for (double x = -halfX; x <= halfX; x += dx)
    for (double y = -halfY; y <= halfY; y += dx)
      {
        tryPush(Element(x, y, 0.0, Element::OUTSIDE));
      }

  const double spacing = sqrt(2.0*itsSizeX*itsSizeY/(sqrt(3.0)*itsTotalNumber));
  printf(" %d elements, ave spacing %f\n", itsTotalNumber, spacing);
}

void GaborArray::backgJitter(Util::Urand& urand) const
{
DOTRACE("GaborArray::backgJitter");

  const double jitter = (itsMinSpacing/16.0);

  const int backgroundIters = 1000;

  const double halfX = 0.5 * itsSizeX;
  const double halfY = 0.5 * itsSizeY;

  for (int niter = 0; niter < backgroundIters; ++niter)
    {
      for (int n = 0; n < itsTotalNumber; ++n)
        {
          if (itsArray[n].type == Element::CONTOUR)
            continue;

          Vec2d v;
          v.x() = itsArray[n].pos.x() + jitter*(urand.fdrawRange(-1.0, 1.0));
          v.y() = itsArray[n].pos.y() + jitter*(urand.fdrawRange(-1.0, 1.0));

          if (v.x() < -halfX) v.x() += itsSizeX;
          if (v.x() >  halfX) v.x() -= itsSizeX;
          if (v.y() < -halfY) v.y() += itsSizeY;
          if (v.y() >  halfY) v.y() -= itsSizeY;

          if (!tooClose(v, n))
            {
              itsArray[n].pos = v;
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
