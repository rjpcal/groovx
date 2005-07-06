/** @file gfx/gxaligner.cc GxBin subclass that aligns its child
    relative to the origin */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Wed Nov 13 12:59:04 2002
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

#ifndef GROOVX_GFX_GXALIGNER_CC_UTC20050626084023_DEFINED
#define GROOVX_GFX_GXALIGNER_CC_UTC20050626084023_DEFINED

#include "gxaligner.h"

#include "gfx/bbox.h"
#include "gfx/canvas.h"

#include "geom/box.h"
#include "geom/rect.h"
#include "geom/txform.h"
#include "geom/vec3.h"

#include "io/reader.h"
#include "io/writer.h"

#include "rutz/debug.h"
GVX_DBG_REGISTER
#include "rutz/trace.h"

using geom::rectd;
using geom::vec2d;
using geom::vec3d;

GxAligner::GxAligner(nub::soft_ref<GxNode> child) :
  GxBin(child),
  itsMode(NATIVE_ALIGNMENT),
  itsCenter(0.0, 0.0)
{}

GxAligner::~GxAligner() throw() {}

vec2d GxAligner::getCenter(const rectd& b) const
{
GVX_TRACE("GxAligner::getCenter");
  switch (itsMode)
    {
    case CENTER_ON_CENTER:    return vec2d::zeros();
    case NW_ON_CENTER:        return vec2d( b.width()/2.0, -b.height()/2.0);
    case NE_ON_CENTER:        return vec2d(-b.width()/2.0, -b.height()/2.0);
    case SW_ON_CENTER:        return vec2d( b.width()/2.0,  b.height()/2.0);
    case SE_ON_CENTER:        return vec2d(-b.width()/2.0,  b.height()/2.0);
    case ARBITRARY_ON_CENTER: return itsCenter;
    case NATIVE_ALIGNMENT:    return b.center();
    }

  GVX_ASSERT(0);
  return vec2d();
}

void GxAligner::doAlignment(Gfx::Canvas& canvas, const rectd& native) const
{
GVX_TRACE("GxAligner::doAlignment");

  if (NATIVE_ALIGNMENT == itsMode) return;

  // This indicates where the center of the object will go
  const vec2d center = getCenter(native) - native.center();

  const vec3d vec(center.x(), center.y(), 0.0);

  canvas.translate(vec);
}

void GxAligner::readFrom(IO::Reader& reader)
{
GVX_TRACE("GxAligner::readFrom");
  reader.readValue("mode", itsMode);
  reader.readValueObj("center", itsCenter);
}

void GxAligner::writeTo(IO::Writer& writer) const
{
GVX_TRACE("GxAligner::writeTo");
  writer.writeValue("mode", itsMode);
  writer.writeValueObj("center", itsCenter);
}

void GxAligner::draw(Gfx::Canvas& canvas) const
{
GVX_TRACE("GxAligner::draw");
  Gfx::MatrixSaver state(canvas);

  doAlignment(canvas, child()->getBoundingBox(canvas));

  child()->draw(canvas);
}

void GxAligner::getBoundingCube(Gfx::Bbox& bbox) const
{
GVX_TRACE("GxAligner::getBoundingCube");

  Gfx::Bbox mybox = bbox.peer();

  child()->getBoundingCube(mybox);

  rectd bounds = mybox.rect();

  const vec2d center = getCenter(bounds);

  bounds.set_center(center);

  bbox.drawRect(bounds);

  dbg_dump(2, bbox.cube());
}

static const char vcid_groovx_gfx_gxaligner_cc_utc20050626084023[] = "$Id$ $HeadURL$";
#endif // !GROOVX_GFX_GXALIGNER_CC_UTC20050626084023_DEFINED
