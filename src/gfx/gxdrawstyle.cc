/** @file gfx/gxdrawstyle.cc GxNode subclass for manipulating the
    line/polygon drawing style */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Wed Aug 29 10:19:31 2001
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

#ifndef GROOVX_GFX_GXDRAWSTYLE_CC_UTC20050626084025_DEFINED
#define GROOVX_GFX_GXDRAWSTYLE_CC_UTC20050626084025_DEFINED

#include "gfx/gxdrawstyle.h"

#include "gfx/canvas.h"

#include "io/reader.h"
#include "io/writer.h"

namespace
{
  const io::version_id GXDRAWSTYLE_SVID = 1;
}

GxDrawStyle::GxDrawStyle() :
  FieldContainer(&sigNodeChanged),
  polygonFill(false),
  pointSize(1.0),
  lineWidth(1.0),
  linePattern(0xFFFF)
{
  setFieldMap(GxDrawStyle::classFields());
}

GxDrawStyle::~GxDrawStyle() noexcept {}

io::version_id GxDrawStyle::class_version_id() const
{
  return GXDRAWSTYLE_SVID;
}

void GxDrawStyle::read_from(io::reader& reader)
{
  reader.ensure_version_id("GxDrawStyle", 1,
                           "Try groovx0.8a7", SRC_POS);

  readFieldsFrom(reader, classFields());
}

void GxDrawStyle::write_to(io::writer& writer) const
{
  writer.ensure_output_version_id("GxDrawStyle", GXDRAWSTYLE_SVID, 1,
                              "Try groovx0.8a7", SRC_POS);

  writeFieldsTo(writer, classFields(), GXDRAWSTYLE_SVID);
}

const FieldMap& GxDrawStyle::classFields()
{
  static const Field FIELD_ARRAY[] =
  {
    Field("polygonFill", &GxDrawStyle::polygonFill, false, false, true, true,
          Field::NEW_GROUP),
    Field("pointSize", &GxDrawStyle::pointSize, 1.0, 0.1, 10.0, 0.1),
    Field("lineWidth", &GxDrawStyle::lineWidth, 1.0, 0.1, 10.0, 0.1),
    Field("linePattern", &GxDrawStyle::linePattern, 0xFFFF, 0, 0xFFFF, 1)
  };

  static FieldMap GXDRAWSTYLE_FIELDS(FIELD_ARRAY);

  return GXDRAWSTYLE_FIELDS;
}

void GxDrawStyle::getBoundingCube(Gfx::Bbox&) const
{
  ; // do nothing, since GxDrawStyle doesn't actually draw anything on-screen
}

void GxDrawStyle::draw(Gfx::Canvas& canvas) const
{
  canvas.setPolygonFill(polygonFill);
  canvas.setPointSize(pointSize);
  canvas.setLineWidth(lineWidth);
  canvas.setLineStipple(static_cast<unsigned short>(linePattern));
}

#endif // !GROOVX_GFX_GXDRAWSTYLE_CC_UTC20050626084025_DEFINED
