/** @file visx/gaborarray.cc spatial array of gabor patches, possibly
    with embedded contours ("snakes") */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Mon May 12 11:15:58 2003
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

#include "gaborarray.h"

#include "geom/geom.h"
#include "geom/rect.h"

#include "gfx/bbox.h"
#include "gfx/canvas.h"
#include "gfx/gxaligner.h"

#include "io/ioproxy.h"
#include "io/reader.h"
#include "io/writer.h"

#include "media/bmapdata.h"
#include "media/imgfile.h"

#include "rutz/error.h"
#include "rutz/rand.h"
#include "rutz/sfmt.h"

#include "visx/gaborpatch.h"
#include "visx/snake.h"

#include <cstdio>

#include "rutz/debug.h"
GVX_DBG_REGISTER
#include "rutz/trace.h"

using geom::vec2i;
using geom::vec2st;
using geom::vec2d;
using geom::vec3d;

using media::bmap_data;

using std::unique_ptr;

using Gfx::Bbox;
using Gfx::Canvas;

namespace
{
  void dumpFrame(const media::bmap_data& bmap)
  {
    GVX_TRACE("<gaborarray.cc>::dumpFrame");

    static int framecount = 0;

    char fname[256];
    snprintf(fname, 256, "frame_%06d.png", framecount++);

    media::save_image(fname, bmap);
  }

  const int GABORARRAY_SVID = 0;
}

GaborArray::GaborArray(double gaborPeriod, double gaborSigma,
                       size_t foregNumber, double foregSpacing,
                       size_t sizeX, size_t sizeY,
                       double gridSpacing,
                       double minSpacing)
  :
  itsForegSeed(0u),
  itsForegNumber(foregNumber),
  itsForegSpacing(foregSpacing),
  itsForegPosX(0),
  itsForegPosY(0),

  itsBackgSeed(0u),
  itsSizeX(sizeX),
  itsSizeY(sizeY),
  itsGridSpacing(gridSpacing),
  itsMinSpacing(minSpacing),
  itsFillResolution(15.0),

  itsThetaSeed(0u),
  itsPhaseSeed(0u),
  itsThetaJitter(0.0),
  itsGaborPeriod(gaborPeriod),
  itsGaborSigma(gaborSigma),
  itsContrastSeed(0u),
  itsContrastJitter(0.0),

  itsArray(),
  itsBmap(),

  itsDumpingFrames(false),
  itsFrameDumpPeriod(20)
{
GVX_TRACE("GaborArray::GaborArray");

  setAlignmentMode(GxAligner::CENTER_ON_CENTER);
  setPercentBorder(0);
}

GaborArray::~GaborArray() noexcept {}

const FieldMap& GaborArray::classFields()
{
  static const Field FIELD_ARRAY[] =
  {
    Field("foregSeed", &GaborArray::itsForegSeed, 0u, 0u, 20000u, 1u,
          Field::NEW_GROUP),
    Field("foregNumber", &GaborArray::itsForegNumber, 24u, 1u, 100u, 1u),
    Field("foregSpacing", &GaborArray::itsForegSpacing,
          45.0, 1.0, 100.0, 1.0),
    Field("foregPosX", &GaborArray::itsForegPosX, 0, -2048, 2048, 1),
    Field("foregPosY", &GaborArray::itsForegPosY, 0, -2048, 2048, 1),

    Field("backgSeed", &GaborArray::itsBackgSeed, 0u, 0u, 20000u, 1u),
    Field("sizeX", &GaborArray::itsSizeX, 512u, 16u, 2048u, 16u),
    Field("sizeY", &GaborArray::itsSizeY, 512u, 16u, 2048u, 16u),
    Field("gridSpacing", &GaborArray::itsGridSpacing, 48., 1., 200., 1.),
    Field("minSpacing", &GaborArray::itsMinSpacing, 36., 1., 200., 1.),
    Field("fillResolution", &GaborArray::itsFillResolution, 15., 1., 50., 1.),

    Field("thetaSeed", &GaborArray::itsThetaSeed, 0u, 0u, 20000u, 1u),
    Field("phaseSeed", &GaborArray::itsPhaseSeed, 0u, 0u, 20000u, 1u),
    Field("thetaJitter", &GaborArray::itsThetaJitter, 0.0, 0.0, 1.0, 0.01),
    Field("gaborPeriod", &GaborArray::itsGaborPeriod, 15.0, 1.0, 50.0, 1.0),
    Field("gaborSigma", &GaborArray::itsGaborSigma, 7.5, 0.5, 25.0, 0.5),
    Field("contrastSeed", &GaborArray::itsContrastSeed, 0u, 0u, 20000u, 1u),
    Field("contrastJitter", &GaborArray::itsContrastJitter, 0.0, 0.0, 2.0, 0.01),

    Field("dumpingFrames", &GaborArray::itsDumpingFrames,
          false, false, true, true, Field::BOOLEAN | Field::TRANSIENT),
    Field("frameDumpPeriod", &GaborArray::itsFrameDumpPeriod,
          20u, 1u, 100u, 1u, Field::TRANSIENT),
  };

  static FieldMap GABORARRAY_FIELDS(FIELD_ARRAY, &GxShapeKit::classFields());

  return GABORARRAY_FIELDS;
}

io::version_id GaborArray::class_version_id() const
{
GVX_TRACE("GaborArray::class_version_id");
  return GABORARRAY_SVID;
}

void GaborArray::read_from(io::reader& reader)
{
GVX_TRACE("GaborArray::read_from");

  readFieldsFrom(reader, classFields());

  reader.read_base_class("GxShapeKit", io::make_proxy<GxShapeKit>(this));
}

void GaborArray::write_to(io::writer& writer) const
{
GVX_TRACE("GaborArray::write_to");

  writeFieldsTo(writer, classFields(), GABORARRAY_SVID);

  writer.write_base_class("GxShapeKit", io::make_const_proxy<GxShapeKit>(this));
}

void GaborArray::saveImage(const char* filename) const
{
GVX_TRACE("GaborArray::saveImage");

  update();

  media::save_image(filename, itsBmap);
}

void GaborArray::saveContourOnlyImage(const char* filename) const
{
GVX_TRACE("GaborArray::saveContourOnlyImage");

  updateForeg();

  const size_t npix = itsSizeX*itsSizeY;

  std::vector<double> win(npix, 0.0);

  for (const auto& elem: itsArray)
    {
      if (elem.type != GaborArrayElement::Type::CONTOUR)
        continue;

      const double theta = geom::rad_0_2pi(elem.theta + M_PI_2);

      const size_t xcenter = size_t(elem.pos.x() + itsSizeX / 2.0 + 0.5);
      const size_t ycenter = size_t(elem.pos.y() + itsSizeY / 2.0 + 0.5);

      const double period = 1.5*itsForegSpacing;

      const GaborPatch& p =
        GaborPatch::lookup(itsForegSpacing/2.0, 2*M_PI/period,
                           theta, 0.0);

      const size_t ps = p.size();

      // bottom left:
      const size_t x0 = std::max(xcenter, ps / 2) - ps / 2;
      const size_t y0 = std::max(ycenter, ps / 2) - ps / 2;
      // top right:
      const size_t x1 = std::min(itsSizeX.val, xcenter + ps - ps / 2);
      const size_t y1 = std::min(itsSizeY.val, ycenter + ps - ps / 2);

      for (size_t y = y0; y < y1; ++y)
        {
          const size_t py = y + ps/2 - ycenter;
          for (size_t x = x0; x < x1; ++x)
            {
              const size_t px = x + ps/2 - xcenter;
              win[x+y*itsSizeX] += p.at(px, py);
            }
        }
    }

  media::bmap_data result(vec2st(itsSizeX, itsSizeY), 8, 1);

  unsigned char* bytes = result.bytes_ptr();

  for (size_t k = 0; k < npix; ++k)
    {
      int val = int(win[k]*255);

      // Threshold anything below 128 here, so that we don't get "ghosts"
      // showing up from the second peaks of the gabor functions.
      if      (val < 128) { val = 0; }
      else if (val > 255) { val = 255; }

      *bytes++ = (unsigned char)(val);
    }

  media::save_image(filename, result);
}

void GaborArray::grGetBoundingBox(Bbox& bbox) const
{
GVX_TRACE("GaborArray::grGetBoundingBox");

  bbox.drawScreenRect(vec3d::zeros(),
                      vec2st(itsSizeX, itsSizeY),
                      vec2d::ones());
}

void GaborArray::grRender(Canvas& canvas) const
{
GVX_TRACE("GaborArray::grRender");

  update();

  canvas.drawPixels(itsBmap, vec3d::zeros(), vec2d::ones());
}

void GaborArray::updateForeg() const
{
GVX_TRACE("GaborArray::updateForeg");

  if (itsForegSeed.ok()
      && itsForegNumber.ok()
      && itsForegSpacing.ok()
      && itsForegPosX.ok()
      && itsForegPosY.ok())
    return;

  itsArray.resize(0);

  rutz::urand urand(itsForegSeed);

  Snake snake(itsForegNumber, itsForegSpacing, urand);

  // pull in elements from the snake
  for (size_t n = 0; n < itsForegNumber; ++n)
    {
      GaborArrayElement e = snake.getElement(n);
      e.pos.x() += itsForegPosX;
      e.pos.y() += itsForegPosY;

      if (!tryPush(e))
        {
          throw rutz::error("foreground elements were "
                            "too close together!\n", SRC_POS);
        }
    }

  itsForegSeed.save();
  itsForegNumber.save();
  itsForegSpacing.save();
  itsForegPosX.save();
  itsForegPosY.save();

  itsBackgSeed.touch(); // to force a redo in updateBackg()

  GVX_ASSERT(itsArray.size() == itsForegNumber);
}

void GaborArray::updateBackg() const
{
GVX_TRACE("GaborArray::updateBackg");

  if (itsBackgSeed.ok()
      && itsSizeX.ok()
      && itsSizeY.ok()
      && itsGridSpacing.ok()
      && itsMinSpacing.ok()
      && itsFillResolution.ok())
    return;

  itsArray.resize(itsForegNumber);

  backgHexGrid();

  const int diffusionCycles = 10;

  rutz::urand urand(itsBackgSeed);

  for (int i = 0; i < diffusionCycles; ++i)
    {
      backgJitter(urand);
      backgFill();
    }

  size_t insideNumber = itsForegNumber;

  for (auto& bgelem: itsArray)
    {
      if (bgelem.type == GaborArrayElement::Type::CONTOUR)
        continue;

      bool inside = true;

      for (size_t i = 0; i < itsForegNumber; ++i)
        {
          const size_t j = (i+1) % itsForegNumber;

          // This is the vector perpendicular to the line connecting
          // contour nodes i and j
          const double Yij = itsArray[i].pos.x() - itsArray[j].pos.x();
          const double Xij = itsArray[j].pos.y() - itsArray[i].pos.y();

          // This is the vector connecting from contour node i to the
          // background node
          const double Xin = bgelem.pos.x() - itsArray[i].pos.x();
          const double Yin = bgelem.pos.y() - itsArray[i].pos.y();

          // If the dot product of those two vectors is less than zero,
          // then the background node is "outside".
          const double vp = Xij*Xin + Yij*Yin;

          if (vp < 0.0) { inside = false; break; }
        }

      if (inside)
        {
          bgelem.type = GaborArrayElement::Type::INSIDE;
          ++insideNumber;
        }
    }

  printf(" FOREG_NUMBER %zu    PATCH_NUMBER %zu    TOTAL_NUMBER %zu\n",
         itsForegNumber.val, insideNumber, itsArray.size());

  itsBackgSeed.save();
  itsSizeX.save();
  itsSizeY.save();
  itsGridSpacing.save();
  itsMinSpacing.save();
  itsFillResolution.save();

  itsThetaSeed.touch(); // to force a redo in updateBmap()
}

media::bmap_data GaborArray::generateBmap(bool doTagLast) const
{
GVX_TRACE("GaborArray::generateBmap");

  const size_t npix = itsSizeX*itsSizeY;

  std::vector<double> win(npix, 0.0);

  rutz::urand thetas(itsThetaSeed);
  rutz::urand phases(itsPhaseSeed);
  rutz::urand contrasts(itsContrastSeed);

  for (size_t i = 0; i < itsArray.size(); ++i)
    {
      const double phi   = 2 * M_PI * phases.fdraw();
      const double rand_theta = 2*M_PI * thetas.fdraw();

      const double theta =
        (itsArray[i].type == GaborArrayElement::Type::CONTOUR)
        ? geom::rad_0_2pi(itsThetaJitter * (rand_theta - M_PI) +
                          (itsArray[i].theta + M_PI_2))
        : rand_theta;

      const size_t xcenter = size_t(itsArray[i].pos.x() + itsSizeX / 2.0 + 0.5);
      const size_t ycenter = size_t(itsArray[i].pos.y() + itsSizeY / 2.0 + 0.5);

      const GaborPatch& p =
        GaborPatch::lookup(itsGaborSigma, 2*M_PI/itsGaborPeriod,
                           theta, phi);

      const size_t ps = p.size();

      // bottom left:
      const size_t x0 = std::max(xcenter, ps / 2) - ps / 2;
      const size_t y0 = std::max(ycenter, ps / 2) - ps / 2;
      // top right:
      const size_t x1 = std::min(itsSizeX.val, xcenter + ps - ps / 2);
      const size_t y1 = std::min(itsSizeY.val, ycenter + ps - ps / 2);

      const double contrast = exp(-itsContrastJitter * contrasts.fdraw());

      for (size_t y = y0; y < y1; ++y)
        {
          const size_t py = y + ps/2 - ycenter;
          for (size_t x = x0; x < x1; ++x)
            {
              const size_t px = x + ps/2 - xcenter;
              win[x+y*itsSizeX] += contrast * p.at(px, py);
            }
        }

      if (doTagLast && i == (itsArray.size()-1))
        {
          const double outer = 0.4 * p.size();
          const double inner = outer - 3;

          for (size_t y = y0; y < y1; ++y)
            for (size_t x = x0; x < x1; ++x)
              {
                const double r = sqrt((x-xcenter)*(x-xcenter) +
                                      (y-ycenter)*(y-ycenter));

                if (r >= inner && r <= outer)
                  {
                    win[x+y*itsSizeX] = 1.0;
                  }
              }
        }
    }

  media::bmap_data result(vec2st(itsSizeX, itsSizeY), 8, 1);

  unsigned char* bytes = result.bytes_ptr();

  bool clip = false;

  for (size_t k = 0; k < npix; ++k)
    {
      int val = int((win[k]+1.0)/2.0*255);

      if      (val < 0)   { clip = true; val = 0; }
      else if (val > 255) { clip = true; val = 255; }

      GVX_ASSERT(val >= 0);
      GVX_ASSERT(val <= 255);

      *bytes++ = (unsigned char)(val);
    }

  if (clip)
    printf("warning: some values were clipped\n");

  return result;
}

void GaborArray::updateBmap() const
{
GVX_TRACE("GaborArray::updateBmap");

  if (itsThetaSeed.ok()
      && itsPhaseSeed.ok()
      && itsThetaJitter.ok()
      && itsGaborPeriod.ok()
      && itsGaborSigma.ok()
      && itsContrastSeed.ok()
      && itsContrastJitter.ok())
    return;

  itsBmap = generateBmap(false);

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
GVX_TRACE("GaborArray::update");

  updateForeg();

  updateBackg();

  updateBmap();
}

bool GaborArray::tryPush(const GaborArrayElement& e) const
{
  if (tooClose(e.pos, size_t(-1))) return false;

  itsArray.push_back(e);

  // this double call is on purpose so that the added elements don't fly by
  // quite so quickly in the resulting movie
  if (itsDumpingFrames)
    {
      const media::bmap_data bmap = generateBmap(true);

      dumpFrame(bmap);
      dumpFrame(bmap);
    }

  return true;
}

bool GaborArray::tooClose(const vec2d& v, size_t except) const
{
  double minSpacingSqr = itsMinSpacing*itsMinSpacing;

  for (size_t n = 0; n < itsArray.size(); ++n)
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
GVX_TRACE("GaborArray::backgHexGrid");

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
          tryPush(GaborArrayElement(x, y, 0.0, GaborArrayElement::Type::OUTSIDE));
        }
    }
}

void GaborArray::backgFill() const
{
GVX_TRACE("GaborArray::backgFill");

  const double dx = itsMinSpacing / itsFillResolution;

  const double halfX = 0.5 * itsSizeX;
  const double halfY = 0.5 * itsSizeY;

  for (double x = -halfX; x <= halfX; x += dx)
    for (double y = -halfY; y <= halfY; y += dx)
      {
        tryPush(GaborArrayElement(x, y, 0.0, GaborArrayElement::Type::OUTSIDE));
      }

  const double spacing = sqrt(2.0*itsSizeX*itsSizeY/(sqrt(3.0)*itsArray.size()));
  printf(" %zu elements, ave spacing %f\n", itsArray.size(), spacing);
}

void GaborArray::backgJitter(rutz::urand& urand) const
{
GVX_TRACE("GaborArray::backgJitter");

  const double jitter = (itsMinSpacing/16.0);

  const unsigned int backgroundIters = 1000u;

  const double halfX = 0.5 * itsSizeX;
  const double halfY = 0.5 * itsSizeY;

  for (unsigned int niter = 0; niter < backgroundIters; ++niter)
    {
      for (size_t n = 0; n < itsArray.size(); ++n)
        {
          if (itsArray[n].type == GaborArrayElement::Type::CONTOUR)
            continue;

          vec2d v;
          v.x() = itsArray[n].pos.x() + jitter*(urand.fdraw_range(-1.0, 1.0));
          v.y() = itsArray[n].pos.y() + jitter*(urand.fdraw_range(-1.0, 1.0));

          if (v.x() < -halfX) v.x() += itsSizeX;
          if (v.x() >  halfX) v.x() -= itsSizeX;
          if (v.y() < -halfY) v.y() += itsSizeY;
          if (v.y() >  halfY) v.y() -= itsSizeY;

          if (!tooClose(v, n))
            {
              itsArray[n].pos = v;
            }
        }

      if (itsDumpingFrames &&
          niter % itsFrameDumpPeriod == 0)
        {
          const media::bmap_data bmap = generateBmap(true);
          dumpFrame(bmap);
        }
    }
}
