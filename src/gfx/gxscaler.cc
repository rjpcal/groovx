///////////////////////////////////////////////////////////////////////
//
// gxscaler.cc
//
// Copyright (c) 2002-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Wed Nov 13 13:04:32 2002
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

#ifndef GXSCALER_CC_DEFINED
#define GXSCALER_CC_DEFINED

#include "gxscaler.h"

#include "gfx/canvas.h"

#include "gx/bbox.h"
#include "gx/box.h"
#include "gx/vec3.h"

#include "util/trace.h"

GxScaler::GxScaler() :
  GxBin(),
  itsMode(NATIVE_SCALING),
  itsWidthFactor(1.0),
  itsHeightFactor(1.0)
{}

GxScaler::GxScaler(Util::Ref<GxNode> child) :
  GxBin(child),
  itsMode(NATIVE_SCALING),
  itsWidthFactor(1.0),
  itsHeightFactor(1.0)
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
  Gfx::Rect<double> native_bbox =
    child()->getBoundingBox(Gfx::Canvas::current());

  double current_width = native_bbox.width() * itsWidthFactor;

  if (new_width == 0.0 || new_width == current_width) return;
  if (itsMode == NATIVE_SCALING) return;

  double new_width_factor = new_width / native_bbox.width();

  double change_factor = new_width_factor / itsWidthFactor;

  itsWidthFactor = new_width_factor;

  if (itsMode == MAINTAIN_ASPECT_SCALING)
    {
      itsHeightFactor *= change_factor;
    }

  this->sigNodeChanged.emit();
}

void GxScaler::setHeight(double new_height)
{
  Gfx::Rect<double> native_bbox =
    child()->getBoundingBox(Gfx::Canvas::current());

  double current_height = native_bbox.height() * itsHeightFactor;

  if (new_height == 0.0 || new_height == current_height) return;
  if (itsMode == NATIVE_SCALING) return;

  double new_height_factor = new_height / native_bbox.height();

  double change_factor = new_height_factor / itsHeightFactor;

  itsHeightFactor = new_height_factor;

  if (itsMode == MAINTAIN_ASPECT_SCALING)
    {
      itsWidthFactor *= change_factor;
    }

  this->sigNodeChanged.emit();
}

void GxScaler::setAspectRatio(double new_aspect_ratio)
{
  double current = aspectRatio();

  if (new_aspect_ratio == 0.0 || new_aspect_ratio == current) return;
  if (itsMode != FREE_SCALING) return;

  double change_factor = new_aspect_ratio / current;

  // By convention, we change only the width to reflect the change in
  // aspect ratio

  itsWidthFactor *= change_factor;

  this->sigNodeChanged.emit();
}

void GxScaler::setWidthFactor(double f)
{
  itsWidthFactor = f;
  this->sigNodeChanged.emit();
}

void GxScaler::setHeightFactor(double f)
{
  itsHeightFactor = f;
  this->sigNodeChanged.emit();
}

void GxScaler::setMaxDim(double new_max_dimension)
{
  if (itsMode == NATIVE_SCALING) return;

  double scaling_factor = new_max_dimension / scaledMaxDim();

  itsWidthFactor *= scaling_factor;
  itsHeightFactor *= scaling_factor;

  this->sigNodeChanged.emit();
}

double GxScaler::aspectRatio() const
{
  return itsMode != FREE_SCALING ? 1.0 :
    (itsHeightFactor != 0.0 ? itsWidthFactor/itsHeightFactor : 0.0);
}

double GxScaler::scaledWidth() const
{
  Gfx::Rect<double> native_bbox =
    child()->getBoundingBox(Gfx::Canvas::current());
  return native_bbox.width() * itsWidthFactor;
}

double GxScaler::scaledHeight() const
{
  Gfx::Rect<double> native_bbox =
    child()->getBoundingBox(Gfx::Canvas::current());
  return native_bbox.height() * itsHeightFactor;
}

double GxScaler::scaledMaxDim() const
{
  return Util::max(scaledWidth(), scaledHeight());
}

void GxScaler::draw(Gfx::Canvas& canvas) const
{
  if (NATIVE_SCALING == itsMode)
    {
      child()->draw(canvas);
    }
  else
    {
      Gfx::MatrixSaver state(canvas);
      canvas.scale(Gfx::Vec3<double>(itsWidthFactor, itsHeightFactor, 1.0));
      child()->draw(canvas);
    }
}

void GxScaler::getBoundingCube(Gfx::Bbox& bbox) const
{
  bbox.push();

  if (NATIVE_SCALING != itsMode)
    bbox.scale(Gfx::Vec3<double>(itsWidthFactor, itsHeightFactor, 1.0));

  child()->getBoundingCube(bbox);

  bbox.pop();

  dbgDump(2, bbox.cube());
}

static const char vcid_gxscaler_cc[] = "$Header$";
#endif // !GXSCALER_CC_DEFINED
