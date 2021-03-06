/** @file gfx/gxsphere.cc GxNode subclass for drawing spheres */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Fri Jul  5 14:20:08 2002
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

#include "gfx/gxsphere.h"

#include "geom/box.h"

#include "gfx/bbox.h"
#include "gfx/canvas.h"

#include "io/reader.h"
#include "io/writer.h"

#include "rutz/trace.h"

namespace
{
  const int GXSPHERE_SVID = 0;
}

const FieldMap& GxSphere::classFields()
{
GVX_TRACE("GxSphere::classFields");
  static const Field FIELD_ARRAY[] =
  {
    Field("radius", &GxSphere::itsRadius, 1.0, 0.0, 10.0, 0.1,
          Field::NEW_GROUP),
    Field("slices", &GxSphere::itsSlices, 10, 1, 100, 1),
    Field("stacks", &GxSphere::itsStacks, 10, 1, 100, 1),
    Field("filled", &GxSphere::itsFilled, true, false, true, true,
          Field::BOOLEAN)
  };

  static FieldMap FIELD_MAP(FIELD_ARRAY);

  return FIELD_MAP;
}

GxSphere* GxSphere::make()
{
GVX_TRACE("GxSphere::make");
 return new GxSphere;
}

GxSphere::GxSphere() :
  FieldContainer(&sigNodeChanged),
  itsRadius(1.0),
  itsSlices(50),
  itsStacks(50),
  itsFilled(true)
{
GVX_TRACE("GxSphere::GxSphere");
  setFieldMap(GxSphere::classFields());
}

GxSphere::~GxSphere() noexcept
{
GVX_TRACE("GxSphere::~GxSphere");
}

io::version_id GxSphere::class_version_id() const
{
GVX_TRACE("GxSphere::class_version_id");
  return GXSPHERE_SVID;
}

void GxSphere::read_from(io::reader& reader)
{
GVX_TRACE("GxSphere::read_from");
  readFieldsFrom(reader, classFields());
}

void GxSphere::write_to(io::writer& writer) const
{
GVX_TRACE("GxSphere::write_to");
  writeFieldsTo(writer, classFields(), GXSPHERE_SVID);
}

/////////////
// actions //
/////////////

void GxSphere::getBoundingCube(Gfx::Bbox& bbox) const
{
GVX_TRACE("GxSphere::getBoundingCube");

  geom::box<double> mybox;

  mybox.set_xx_yy_zz(-itsRadius, itsRadius,
                     -itsRadius, itsRadius,
                     -itsRadius, itsRadius);

  bbox.drawBox(mybox);
}

void GxSphere::draw(Gfx::Canvas& canvas) const
{
GVX_TRACE("GxSphere::draw");

  canvas.drawSphere(itsRadius, itsSlices, itsStacks, itsFilled);
}
