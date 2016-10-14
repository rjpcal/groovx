/** @file visx/fixpt.cc graphic object for fixation crosses */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Mon Jan  4 08:00:00 1999
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

#ifndef GROOVX_VISX_FIXPT_CC_UTC20050626084016_DEFINED
#define GROOVX_VISX_FIXPT_CC_UTC20050626084016_DEFINED

#include "visx/fixpt.h"

#include "geom/vec2.h"

#include "gfx/bbox.h"
#include "gfx/canvas.h"

#include "io/ioproxy.h"
#include "io/reader.h"
#include "io/writer.h"

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

///////////////////////////////////////////////////////////////////////
//
// File scope data
//
///////////////////////////////////////////////////////////////////////

namespace
{
  const io::version_id FIXPT_SVID = 3;
}

///////////////////////////////////////////////////////////////////////
//
// FixPt member functions
//
///////////////////////////////////////////////////////////////////////

const FieldMap& FixPt::classFields()
{
  static const Field FIELD_ARRAY[] =
  {
    Field("length", &FixPt::itsLength, 0.1, 0.0, 10.0, 0.1, Field::NEW_GROUP),
    Field("width", &FixPt::itsWidth, 1, 1, 100, 1)
  };

  static FieldMap FIXPT_FIELDS(FIELD_ARRAY, &GxShapeKit::classFields());

  return FIXPT_FIELDS;
}

FixPt* FixPt::make()
{
GVX_TRACE("FixPt::make");
  return new FixPt;
}

FixPt::FixPt(double len, int wid) :
  itsLength(len), itsWidth(wid)
{
  setFieldMap(FixPt::classFields());
}

FixPt::~FixPt() throw() {}

io::version_id FixPt::class_version_id() const
{
GVX_TRACE("FixPt::class_version_id");
  return FIXPT_SVID;
}

void FixPt::read_from(io::reader& reader)
{
GVX_TRACE("FixPt::read_from");

  reader.ensure_version_id("FixPt", 3,
                           "Try cvs tag xml_conversion_20040526",
                           SRC_POS);

  readFieldsFrom(reader, classFields());

  reader.read_base_class("GxShapeKit", io::make_proxy<GxShapeKit>(this));
}

void FixPt::write_to(io::writer& writer) const
{
GVX_TRACE("FixPt::write_to");

  writer.ensure_output_version_id("FixPt", FIXPT_SVID, 3,
                              "Try groovx0.8a4", SRC_POS);

  writeFieldsTo(writer, classFields(), FIXPT_SVID);

  writer.write_base_class("GxShapeKit", io::make_const_proxy<GxShapeKit>(this));
}

void FixPt::grGetBoundingBox(Gfx::Bbox& bbox) const
{
GVX_TRACE("FixPt::grGetBoundingBox");

  bbox.vertex2(geom::vec2<double>(-itsLength/2.0, -itsLength/2.0));
  bbox.vertex2(geom::vec2<double>(+itsLength/2.0, +itsLength/2.0));
}

void FixPt::grRender(Gfx::Canvas& canvas) const
{
GVX_TRACE("FixPt::grRender");

  Gfx::AttribSaver saver(canvas);

  canvas.setLineWidth(itsWidth);

  {
    Gfx::LinesBlock block(canvas);

    canvas.vertex2(geom::vec2<double>(0.0, -itsLength/2.0));
    canvas.vertex2(geom::vec2<double>(0.0, itsLength/2.0));

    canvas.vertex2(geom::vec2<double>(-itsLength/2.0, 0.0));
    canvas.vertex2(geom::vec2<double>(itsLength/2.0, 0.0));
  }
}

#endif // !GROOVX_VISX_FIXPT_CC_UTC20050626084016_DEFINED
