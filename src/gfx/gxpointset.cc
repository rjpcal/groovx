///////////////////////////////////////////////////////////////////////
//
// gxpointset.cc
//
// Copyright (c) 2002-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Fri Jul  5 12:46:27 2002
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

#ifndef GXPOINTSET_CC_DEFINED
#define GXPOINTSET_CC_DEFINED

#include "gfx/gxpointset.h"

#include "geom/box.h"

#include "gfx/bbox.h"
#include "gfx/canvas.h"

#include "io/reader.h"
#include "io/writer.h"

#include "util/trace.h"

namespace
{
  const int GXPOINTSET_SVID = 0;
}

const FieldMap& GxPointSet::classFields()
{
DOTRACE("GxPointSet::classFields");
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
DOTRACE("GxPointSet::make");
 return new GxPointSet;
}

GxPointSet::GxPointSet() :
  FieldContainer(&sigNodeChanged),
  itsPoints()
{
DOTRACE("GxPointSet::GxPointSet");
  setFieldMap(GxPointSet::classFields());
}

GxPointSet::~GxPointSet() throw()
{
DOTRACE("GxPointSet::~GxPointSet");
}

IO::VersionId GxPointSet::serialVersionId() const
{
DOTRACE("GxPointSet::serialVersionId");
  return GXPOINTSET_SVID;
}

void GxPointSet::readFrom(IO::Reader& reader)
{
DOTRACE("GxPointSet::readFrom");
  readFieldsFrom(reader, classFields());
}

void GxPointSet::writeTo(IO::Writer& writer) const
{
DOTRACE("GxPointSet::writeTo");
  writeFieldsTo(writer, classFields(), GXPOINTSET_SVID);
}

/////////////
// actions //
/////////////

void GxPointSet::getBoundingCube(Gfx::Bbox& bbox) const
{
DOTRACE("GxPointSet::getBoundingCube");

  for (unsigned int i = 0; i < itsPoints.array_size(); ++i)
    bbox.vertex3(itsPoints.array_at(i));
}

void GxPointSet::draw(Gfx::Canvas& canvas) const
{
DOTRACE("GxPointSet::draw");

  canvas.begin(Gfx::Canvas::VertexStyle(itsStyle));

  for (unsigned int i = 0; i < itsPoints.array_size(); ++i)
    canvas.vertex3(itsPoints.array_at(i));

  canvas.end();
}

static const char vcid_gxpointset_cc[] = "$Id$ $URL$";
#endif // !GXPOINTSET_CC_DEFINED
