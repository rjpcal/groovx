/** @file gfx/gxcolor.cc GxNode class for manipulating the current RGBA
    color */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Wed Aug 22 17:45:52 2001
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

#ifndef GROOVX_GFX_GXCOLOR_CC_UTC20050626084025_DEFINED
#define GROOVX_GFX_GXCOLOR_CC_UTC20050626084025_DEFINED

#include "gfx/gxcolor.h"

#include "gfx/canvas.h"

#include "io/reader.h"
#include "io/writer.h"

namespace
{
  const io::version_id GXCOLOR_SVID = 1;
}

GxColor::GxColor() :
  FieldContainer(&sigNodeChanged),
  rgbaColor(1.0)
{
  setFieldMap(GxColor::classFields());
}

GxColor::~GxColor() noexcept {}

io::version_id GxColor::class_version_id() const
{
  return GXCOLOR_SVID;
}

void GxColor::read_from(io::reader& reader)
{
  reader.ensure_version_id("GxColor", 1,
                           "Try groovx0.8a7", SRC_POS);

  readFieldsFrom(reader, classFields());
}

void GxColor::write_to(io::writer& writer) const
{
  writer.ensure_output_version_id("GxColor", GXCOLOR_SVID, 1,
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

#endif // !GROOVX_GFX_GXCOLOR_CC_UTC20050626084025_DEFINED
