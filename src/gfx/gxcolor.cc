///////////////////////////////////////////////////////////////////////
//
// gxcolor.cc
//
// Copyright (c) 2001-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Wed Aug 22 17:45:52 2001
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

#ifndef GXCOLOR_CC_DEFINED
#define GXCOLOR_CC_DEFINED

#include "gfx/gxcolor.h"

#include "gfx/canvas.h"

#include "io/reader.h"
#include "io/writer.h"

namespace
{
  const IO::VersionId GXCOLOR_SVID = 1;
}

GxColor::GxColor() :
  FieldContainer(&sigNodeChanged),
  rgbaColor(1.0)
{
  setFieldMap(GxColor::classFields());
}

GxColor::~GxColor() throw() {}

IO::VersionId GxColor::serialVersionId() const
{
  return GXCOLOR_SVID;
}

void GxColor::readFrom(IO::Reader& reader)
{
  reader.ensureReadVersionId("GxColor", 1,
                             "Try groovx0.8a7", SRC_POS);

  readFieldsFrom(reader, classFields());
}

void GxColor::writeTo(IO::Writer& writer) const
{
  writer.ensureWriteVersionId("GxColor", GXCOLOR_SVID, 1,
                              "Try groovx0.8a7", SRC_POS);

  writeFieldsTo(writer, classFields(), GXCOLOR_SVID);
}

const FieldMap& GxColor::classFields()
{
  static const Field FIELD_ARRAY[] =
  {
    Field("rgba", Field::ValueType(), &GxColor::rgbaColor,
          "0.0 0.0 0.0 0.0", "0.0 0.0 0.0 0.0", "1.0 1.0 1.0 1.0",
          "0.01 0.01 0.01 0.01", Field::NEW_GROUP | Field::MULTI)
  };

  static FieldMap GXCOLOR_FIELDS(FIELD_ARRAY);

  return GXCOLOR_FIELDS;
}

void GxColor::getBoundingCube(Gfx::Bbox&) const
{
  ; // do nothing, since GxColor doesn't actually draw anything on-screen
}

void GxColor::draw(Gfx::Canvas& canvas) const
{
  canvas.setColor(rgbaColor);
}

static const char vcid_gxcolor_cc[] = "$Id$ $URL$";
#endif // !GXCOLOR_CC_DEFINED
