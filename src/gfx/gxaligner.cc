///////////////////////////////////////////////////////////////////////
//
// gxaligner.cc
//
// Copyright (c) 2002-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Wed Nov 13 12:59:04 2002
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

#ifndef GXALIGNER_CC_DEFINED
#define GXALIGNER_CC_DEFINED

#include "gxaligner.h"

#include "gfx/bbox.h"
#include "gfx/canvas.h"

#include "geom/box.h"
#include "geom/rect.h"
#include "geom/txform.h"
#include "geom/vec3.h"

#include "util/debug.h"
DBG_REGISTER
#include "util/trace.h"

GxAligner::GxAligner(Nub::SoftRef<GxNode> child) :
  GxBin(child),
  itsMode(NATIVE_ALIGNMENT),
  itsCenter(0.0, 0.0)
{}

geom::vec2<double> GxAligner::getCenter(const geom::rect<double>& bounds) const
{
DOTRACE("GxAligner::getCenter");
  switch (itsMode)
    {
    case CENTER_ON_CENTER:
      return geom::vec2<double>(0.0, 0.0);
    case NW_ON_CENTER:
      return geom::vec2<double>(bounds.width()/2.0, -bounds.height()/2.0);
    case NE_ON_CENTER:
      return geom::vec2<double>(-bounds.width()/2.0, -bounds.height()/2.0);
    case SW_ON_CENTER:
      return geom::vec2<double>(bounds.width()/2.0, bounds.height()/2.0);
    case SE_ON_CENTER:
      return geom::vec2<double>(-bounds.width()/2.0, bounds.height()/2.0);
    case ARBITRARY_ON_CENTER:
      return itsCenter;
    }
  // NATIVE_ALIGNMENT
  return bounds.center();
}

void GxAligner::doAlignment(Gfx::Canvas& canvas,
                            const geom::rect<double>& native) const
{
DOTRACE("GxAligner::doAlignment");

  if (NATIVE_ALIGNMENT == itsMode) return;

  // This indicates where the center of the object will go
  geom::vec2<double> center = getCenter(native);

  center -= native.center();

  geom::vec3<double> vec(center.x(), center.y(), 0.0);

  canvas.translate(vec);
}


void GxAligner::draw(Gfx::Canvas& canvas) const
{
DOTRACE("GxAligner::draw");
  Gfx::MatrixSaver state(canvas);

  doAlignment(canvas, child()->getBoundingBox(canvas));

  child()->draw(canvas);
}

void GxAligner::getBoundingCube(Gfx::Bbox& bbox) const
{
DOTRACE("GxAligner::getBoundingCube");

  Gfx::Bbox mybox = bbox.peer();

  child()->getBoundingCube(mybox);

  geom::rect<double> bounds = mybox.rect();

  geom::vec2<double> center = getCenter(bounds);

  bounds.set_center(center);

  bbox.drawRect(bounds);

  dbg_dump(2, bbox.cube());
}

static const char vcid_gxaligner_cc[] = "$Header$";
#endif // !GXALIGNER_CC_DEFINED
