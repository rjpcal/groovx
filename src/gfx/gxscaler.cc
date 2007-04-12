/** @file gfx/gxscaler.cc GxBin subclass that rescales its child node */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Wed Nov 13 13:04:32 2002
// commit: $Id$
// $HeadURL$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
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

#ifndef GROOVX_GFX_GXSCALER_CC_UTC20050626084023_DEFINED
#define GROOVX_GFX_GXSCALER_CC_UTC20050626084023_DEFINED

#include "gxscaler.h"

#include "geom/box.h"
#include "geom/rect.h"
#include "geom/vec3.h"

#include "gfx/bbox.h"
#include "gfx/canvas.h"
#include "gfx/glcanvas.h"

#include "io/reader.h"
#include "io/writer.h"

#include "rutz/algo.h"
#include "rutz/error.h"

#include "rutz/trace.h"

using geom::rectd;

GxScaler::GxScaler() :
  GxBin(),
  itsMode(NATIVE_SCALING),
  itsWidthFactor(1.0),
  itsHeightFactor(1.0),
  itsAspectRatio(1.0)
{}

GxScaler::GxScaler(nub::ref<GxNode> child) :
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
GVX_TRACE("GxScaler::setMode");

  itsMode = new_mode;
  this->sigNodeChanged.emit();
}

void GxScaler::setWidth(double new_width)
{
GVX_TRACE("GxScaler::setWidth");
  if (itsMode == NATIVE_SCALING) return;

  const rectd native_bbox =
    child()->getBoundingBox(*(GLCanvas::getCurrent()));

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
GVX_TRACE("GxScaler::setHeight");
  if (itsMode == NATIVE_SCALING) return;

  const rectd native_bbox =
    child()->getBoundingBox(*(GLCanvas::getCurrent()));

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
GVX_TRACE("GxScaler::setAspectRatio");
  if (itsMode != FREE_SCALING) return;

  itsAspectRatio = new_aspect_ratio;

  this->sigNodeChanged.emit();
}

void GxScaler::setWidthFactor(double f)
{
GVX_TRACE("GxScaler::setWidthFactor");
  itsWidthFactor = f;
  this->sigNodeChanged.emit();
}

void GxScaler::setHeightFactor(double f)
{
GVX_TRACE("GxScaler::setHeightFactor");
  itsHeightFactor = f;
  this->sigNodeChanged.emit();
}

void GxScaler::setMaxDim(double new_max_dimension)
{
GVX_TRACE("GxScaler::setMaxDim");
  if (itsMode == NATIVE_SCALING) return;

  double scaling_factor = new_max_dimension / scaledMaxDim();

  itsWidthFactor *= scaling_factor;
  itsHeightFactor *= scaling_factor;

  this->sigNodeChanged.emit();
}

double GxScaler::aspectRatio() const
{
GVX_TRACE("GxScaler::aspectRatio");
  return itsMode != FREE_SCALING ? 1.0 : itsAspectRatio;
}

double GxScaler::scaledWidth() const
{
GVX_TRACE("GxScaler::scaledWidth");
  const rectd native_bbox =
    child()->getBoundingBox(*(GLCanvas::getCurrent()));
  return native_bbox.width() * itsWidthFactor * aspectRatio();
}

double GxScaler::scaledHeight() const
{
GVX_TRACE("GxScaler::scaledHeight");
  const rectd native_bbox =
    child()->getBoundingBox(*(GLCanvas::getCurrent()));
  return native_bbox.height() * itsHeightFactor;
}

double GxScaler::scaledMaxDim() const
{
GVX_TRACE("GxScaler::scaledMaxDim");
  return rutz::max(scaledWidth(), scaledHeight());
}

void GxScaler::read_from(io::reader& reader)
{
GVX_TRACE("GxScaler::read_from");
  reader.read_value("mode", itsMode);
  reader.read_value("widthFactor", itsWidthFactor);
  reader.read_value("heightFactor", itsHeightFactor);
  reader.read_value("aspectRatio", itsAspectRatio);
}

void GxScaler::write_to(io::writer& writer) const
{
GVX_TRACE("GxScaler::write_to");
  writer.write_value("mode", itsMode);
  writer.write_value("widthFactor", itsWidthFactor);
  writer.write_value("heightFactor", itsHeightFactor);
  writer.write_value("aspectRatio", itsAspectRatio);
}

void GxScaler::draw(Gfx::Canvas& canvas) const
{
GVX_TRACE("GxScaler::draw");
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
GVX_TRACE("GxScaler::getBoundingCube");
  bbox.push();

  if (NATIVE_SCALING != itsMode)
    bbox.scale(geom::vec3<double>(itsWidthFactor * aspectRatio(),
                                  itsHeightFactor,
                                  1.0));

  child()->getBoundingCube(bbox);

  bbox.pop();

  dbg_dump(2, bbox.cube());
}

static const char __attribute__((used)) vcid_groovx_gfx_gxscaler_cc_utc20050626084023[] = "$Id$ $HeadURL$";
#endif // !GROOVX_GFX_GXSCALER_CC_UTC20050626084023_DEFINED
