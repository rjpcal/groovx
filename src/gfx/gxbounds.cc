///////////////////////////////////////////////////////////////////////
//
// gxbounds.cc
//
// Copyright (c) 2002-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Wed Nov 13 13:34:18 2002
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

#ifndef GROOVX_GFX_GXBOUNDS_CC_UTC20050626084023_DEFINED
#define GROOVX_GFX_GXBOUNDS_CC_UTC20050626084023_DEFINED

#include "gxbounds.h"

#include "geom/box.h"
#include "geom/rect.h"

#include "gfx/bbox.h"
#include "gfx/canvas.h"
#include "gfx/rgbacolor.h"

#include "io/reader.h"
#include "io/writer.h"

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

GxBounds::GxBounds(nub::soft_ref<GxNode> child) :
  GxBin(child),
  isItVisible(false),
  isItAnimated(true),
  itsPercentBorder(4),
  itsStipple(0x0F0F), // 0000111100001111
  itsMask(0x3333)     // 0011001100110011
{}

GxBounds::~GxBounds() throw() {}

void GxBounds::readFrom(IO::Reader& reader)
{
GVX_TRACE("GxBounds::readFrom");
  reader.readValue("isVisible", isItVisible);
  reader.readValue("isAnimated", isItAnimated);
  reader.readValue("percentBorder", itsPercentBorder);
}

void GxBounds::writeTo(IO::Writer& writer) const
{
GVX_TRACE("GxBounds::writeTo");
  writer.writeValue("isVisible", isItVisible);
  writer.writeValue("isAnimated", isItAnimated);
  writer.writeValue("percentBorder", itsPercentBorder);
}

void GxBounds::draw(Gfx::Canvas& canvas) const
{
GVX_TRACE("GxBounds::draw");

  child()->draw(canvas);

  if (isItVisible)
    {
      Gfx::Bbox bbox(canvas);
      const double s = 1.0 + itsPercentBorder/100.0;
      bbox.scale(geom::vec3<double>(s,s,s));
      child()->getBoundingCube(bbox);

      geom::rect<double> bounds = bbox.rect();

      if (isItAnimated)
        {
          itsStipple ^= itsMask;
          itsMask = ~itsMask;
        }

      Gfx::AttribSaver saver(canvas);

      canvas.setColor(Gfx::RgbaColor(0.0, 0.0, 1.0, 1.0));

      canvas.setLineWidth(1.0);
      canvas.setLineStipple(itsStipple);
      canvas.setPolygonFill(false);

      canvas.drawRect(bounds);

      canvas.setPointSize(4.0);

      {
        Gfx::PointsBlock block(canvas);

        canvas.vertex2(bounds.bottom_left());
        canvas.vertex2(bounds.bottom_right());
        canvas.vertex2(bounds.top_right());
        canvas.vertex2(bounds.top_left());
        canvas.vertex2(bounds.center());
      }
    }
}

void GxBounds::getBoundingCube(Gfx::Bbox& bbox) const
{
GVX_TRACE("GxBounds::getBoundingCube");

  bbox.push();

  dbg_eval(3, itsPercentBorder);

  const double s = 1.0 + itsPercentBorder/100.0;

  bbox.scale(geom::vec3<double>(s,s,s));

  child()->getBoundingCube(bbox);

  bbox.pop();

  dbg_dump(2, bbox.cube());
}

static const char vcid_groovx_gfx_gxbounds_cc_utc20050626084023[] = "$Id$ $HeadURL$";
#endif // !GROOVX_GFX_GXBOUNDS_CC_UTC20050626084023_DEFINED
