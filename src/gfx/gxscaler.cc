///////////////////////////////////////////////////////////////////////
//
// gxscaler.cc
//
// Copyright (c) 2002-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Wed Nov 13 13:04:32 2002
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

#ifndef GXSCALER_CC_DEFINED
#define GXSCALER_CC_DEFINED

#include "gxscaler.h"

#include "geom/box.h"
#include "geom/rect.h"
#include "geom/vec3.h"

#include "gfx/bbox.h"
#include "gfx/canvas.h"
#include "gfx/toglet.h"

#include "io/reader.h"
#include "io/writer.h"

#include "util/algo.h"
#include "util/error.h"

#include "util/trace.h"

using geom::rectd;

GxScaler::GxScaler() :
  GxBin(),
  itsMode(NATIVE_SCALING),
  itsWidthFactor(1.0),
  itsHeightFactor(1.0),
  itsAspectRatio(1.0)
{}

GxScaler::GxScaler(Nub::Ref<GxNode> child) :
  GxBin(child),
  itsMode(NATIVE_SCALING),
  itsWidthFactor(1.0),
  itsHeightFactor(1.0),
  itsAspectRatio(1.0)
{}

GxScaler* GxScaler::make()
{
  return new GxScaler;
}

void GxScaler::setMode(Mode new_mode)
{
DOTRACE("GxScaler::setMode");

  itsMode = new_mode;
  this->sigNodeChanged.emit();
}

void GxScaler::setWidth(double new_width)
{
DOTRACE("GxScaler::setWidth");
  if (itsMode == NATIVE_SCALING) return;

  const rectd native_bbox =
    child()->getBoundingBox(*(Toglet::getCurrent()->getCanvas()));

  const double current_width = native_bbox.width() * itsWidthFactor;

  if (new_width == 0.0 || new_width == current_width) return;

  const double new_width_factor = new_width / native_bbox.width();

  const double change_factor = new_width_factor / itsWidthFactor;

  itsWidthFactor = new_width_factor;

  if (itsMode == MAINTAIN_ASPECT_SCALING)
    {
      itsHeightFactor *= change_factor;
    }

  this->sigNodeChanged.emit();
}

void GxScaler::setHeight(double new_height)
{
DOTRACE("GxScaler::setHeight");
  if (itsMode == NATIVE_SCALING) return;

  const rectd native_bbox =
    child()->getBoundingBox(*(Toglet::getCurrent()->getCanvas()));

  const double current_height = native_bbox.height() * itsHeightFactor;

  if (new_height == 0.0 || new_height == current_height) return;

  const double new_height_factor = new_height / native_bbox.height();

  const double change_factor = new_height_factor / itsHeightFactor;

  itsHeightFactor = new_height_factor;

  if (itsMode == MAINTAIN_ASPECT_SCALING)
    {
      itsWidthFactor *= change_factor;
    }

  this->sigNodeChanged.emit();
}

void GxScaler::setAspectRatio(double new_aspect_ratio)
{
DOTRACE("GxScaler::setAspectRatio");
  if (itsMode != FREE_SCALING) return;

  itsAspectRatio = new_aspect_ratio;

  this->sigNodeChanged.emit();
}

void GxScaler::setWidthFactor(double f)
{
DOTRACE("GxScaler::setWidthFactor");
  itsWidthFactor = f;
  this->sigNodeChanged.emit();
}

void GxScaler::setHeightFactor(double f)
{
DOTRACE("GxScaler::setHeightFactor");
  itsHeightFactor = f;
  this->sigNodeChanged.emit();
}

void GxScaler::setMaxDim(double new_max_dimension)
{
DOTRACE("GxScaler::setMaxDim");
  if (itsMode == NATIVE_SCALING) return;

  double scaling_factor = new_max_dimension / scaledMaxDim();

  itsWidthFactor *= scaling_factor;
  itsHeightFactor *= scaling_factor;

  this->sigNodeChanged.emit();
}

double GxScaler::aspectRatio() const
{
DOTRACE("GxScaler::aspectRatio");
  return itsMode != FREE_SCALING ? 1.0 : itsAspectRatio;
}

double GxScaler::scaledWidth() const
{
DOTRACE("GxScaler::scaledWidth");
  const rectd native_bbox =
    child()->getBoundingBox(*(Toglet::getCurrent()->getCanvas()));
  return native_bbox.width() * itsWidthFactor * aspectRatio();
}

double GxScaler::scaledHeight() const
{
DOTRACE("GxScaler::scaledHeight");
  const rectd native_bbox =
    child()->getBoundingBox(*(Toglet::getCurrent()->getCanvas()));
  return native_bbox.height() * itsHeightFactor;
}

double GxScaler::scaledMaxDim() const
{
DOTRACE("GxScaler::scaledMaxDim");
  return rutz::max(scaledWidth(), scaledHeight());
}

void GxScaler::readFrom(IO::Reader& reader)
{
DOTRACE("GxScaler::readFrom");
  reader.readValue("mode", itsMode);
  reader.readValue("widthFactor", itsWidthFactor);
  reader.readValue("heightFactor", itsHeightFactor);
  reader.readValue("aspectRatio", itsAspectRatio);
}

void GxScaler::writeTo(IO::Writer& writer) const
{
DOTRACE("GxScaler::writeTo");
  writer.writeValue("mode", itsMode);
  writer.writeValue("widthFactor", itsWidthFactor);
  writer.writeValue("heightFactor", itsHeightFactor);
  writer.writeValue("aspectRatio", itsAspectRatio);
}

void GxScaler::draw(Gfx::Canvas& canvas) const
{
DOTRACE("GxScaler::draw");
  if (NATIVE_SCALING == itsMode)
    {
      child()->draw(canvas);
    }
  else
    {
      Gfx::MatrixSaver state(canvas);
      canvas.scale(geom::vec3<double>(itsWidthFactor * aspectRatio(),
                                      itsHeightFactor,
                                      1.0));
      child()->draw(canvas);
    }
}

void GxScaler::getBoundingCube(Gfx::Bbox& bbox) const
{
DOTRACE("GxScaler::getBoundingCube");
  bbox.push();

  if (NATIVE_SCALING != itsMode)
    bbox.scale(geom::vec3<double>(itsWidthFactor * aspectRatio(),
                                  itsHeightFactor,
                                  1.0));

  child()->getBoundingCube(bbox);

  bbox.pop();

  dbg_dump(2, bbox.cube());
}

static const char vcid_gxscaler_cc[] = "$Id$ $URL$";
#endif // !GXSCALER_CC_DEFINED
