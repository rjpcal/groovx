/** @file gfx/gxpointset.cc GxNode subclass for drawing sets of points */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Fri Jul  5 12:46:27 2002
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

#include "gfx/gxpointset.h"

#include "geom/box.h"

#include "gfx/bbox.h"
#include "gfx/canvas.h"

#include "io/reader.h"
#include "io/writer.h"

#include "rutz/trace.h"

namespace
{
  const int GXPOINTSET_SVID = 0;
}

const FieldMap& GxPointSet::classFields()
{
GVX_TRACE("GxPointSet::classFields");
  static const Field FIELD_ARRAY[] =
  {
    Field("points", Field::ValueType(), &GxPointSet::itsPoints,
          "", "", "", "", Field::NEW_GROUP | Field::STRING),
    Field("style", &GxPointSet::itsStyle, 0, 0, 10, 1),
  };

  static FieldMap FIELD_MAP(FIELD_ARRAY);

  return FIELD_MAP;
}

GxPointSet* GxPointSet::make()
{
GVX_TRACE("GxPointSet::make");
 return new GxPointSet;
}

GxPointSet::GxPointSet() :
  FieldContainer(&sigNodeChanged),
  itsPoints(),
  itsStyle(int(Gfx::Canvas::VertexStyle::LINE_STRIP))
{
GVX_TRACE("GxPointSet::GxPointSet");
  setFieldMap(GxPointSet::classFields());
}

GxPointSet::~GxPointSet() noexcept
{
GVX_TRACE("GxPointSet::~GxPointSet");
}

io::version_id GxPointSet::class_version_id() const
{
GVX_TRACE("GxPointSet::class_version_id");
  return GXPOINTSET_SVID;
}

void GxPointSet::read_from(io::reader& reader)
{
GVX_TRACE("GxPointSet::read_from");
  readFieldsFrom(reader, classFields());
}

void GxPointSet::write_to(io::writer& writer) const
{
GVX_TRACE("GxPointSet::write_to");
  writeFieldsTo(writer, classFields(), GXPOINTSET_SVID);
}

/////////////
// actions //
/////////////

void GxPointSet::getBoundingCube(Gfx::Bbox& bbox) const
{
GVX_TRACE("GxPointSet::getBoundingCube");

  for (unsigned int i = 0; i < itsPoints.array_size(); ++i)
    bbox.vertex3(itsPoints.array_at(i));
}

void GxPointSet::draw(Gfx::Canvas& canvas) const
{
GVX_TRACE("GxPointSet::draw");

  canvas.begin(Gfx::Canvas::VertexStyle(itsStyle));

  for (unsigned int i = 0; i < itsPoints.array_size(); ++i)
    canvas.vertex3(itsPoints.array_at(i));

  canvas.end();
}
