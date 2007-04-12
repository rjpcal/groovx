/** @file visx/maskhatch.cc graphic object for cross-hatched visual masks */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Thu Sep 23 15:49:58 1999
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

#ifndef GROOVX_VISX_MASKHATCH_CC_UTC20050626084015_DEFINED
#define GROOVX_VISX_MASKHATCH_CC_UTC20050626084015_DEFINED

#include "visx/maskhatch.h"

#include "geom/rect.h"

#include "gfx/bbox.h"
#include "gfx/canvas.h"
#include "gfx/gxaligner.h"

#include "io/ioproxy.h"
#include "io/reader.h"
#include "io/writer.h"

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

namespace
{
  const io::version_id MASKHATCH_SVID = 3;
}

const FieldMap& MaskHatch::classFields()
{
  static const Field MASK_FINFOS[] =
  {
    Field("numLines", &MaskHatch::itsNumLines, 5, 0, 25, 1, Field::NEW_GROUP),
    Field("lineWidth", &MaskHatch::itsLineWidth, 1, 0, 25, 1, Field::CHECKED)
  };

  static FieldMap MASK_FIELDS(MASK_FINFOS, &GxShapeKit::classFields());

  return MASK_FIELDS;
}

MaskHatch* MaskHatch::make()
{
GVX_TRACE("MaskHatch::make");
  return new MaskHatch;
}

MaskHatch::MaskHatch () :
  GxShapeKit(),
  itsNumLines(10),
  itsLineWidth(1)
{
GVX_TRACE("MaskHatch::MaskHatch");

  sigNodeChanged.connect(this, &MaskHatch::update);

  setFieldMap(MaskHatch::classFields());

  setAlignmentMode(GxAligner::CENTER_ON_CENTER);
  dbg_eval(3, getAlignmentMode());
}

MaskHatch::~MaskHatch() throw()
{
GVX_TRACE("MaskHatch::~MaskHatch");
}

io::version_id MaskHatch::class_version_id() const
{
GVX_TRACE("MaskHatch::class_version_id");
 return MASKHATCH_SVID;
}

void MaskHatch::read_from(io::reader& reader)
{
GVX_TRACE("MaskHatch::read_from");

  reader.ensure_version_id("MaskHatch", 3,
                           "Try cvs tag xml_conversion_20040526",
                           SRC_POS);

  readFieldsFrom(reader, classFields());

  reader.read_base_class("GxShapeKit", io::make_proxy<GxShapeKit>(this));
}

void MaskHatch::write_to(io::writer& writer) const
{
GVX_TRACE("MaskHatch::write_to");

  writer.ensure_output_version_id("MaskHatch",
                              MASKHATCH_SVID, 3,
                              "Try groovx0.8a4", SRC_POS);

  writeFieldsTo(writer, classFields(), MASKHATCH_SVID);

  writer.write_base_class("GxShapeKit", io::make_const_proxy<GxShapeKit>(this));
}

void MaskHatch::update()
{
  setPercentBorder(itsLineWidth/2 + 2);
}

void MaskHatch::grGetBoundingBox(Gfx::Bbox& bbox) const
{
GVX_TRACE("MaskHatch::grGetBoundingBox");

  bbox.drawRect(geom::rect<double>::lbwh(0.0, 0.0, 1.0, 1.0));
}

void MaskHatch::grRender(Gfx::Canvas& canvas) const
{
GVX_TRACE("MaskHatch::grRender");

  if (itsNumLines == 0) return;

  Gfx::AttribSaver attribSaver(canvas);

  canvas.setLineWidth(itsLineWidth);

  Gfx::LinesBlock b(canvas);

  using geom::vec2d;

  for (int i = 0; i < itsNumLines; ++i)
    {
      double position = double(i)/itsNumLines;

      // horizontal line
      canvas.vertex2(vec2d(0.0, position));
      canvas.vertex2(vec2d(1.0, position));

      // vertical line
      canvas.vertex2(vec2d(position, 0.0));
      canvas.vertex2(vec2d(position, 1.0));

      // lines with slope = 1
      canvas.vertex2(vec2d(0.0, position));
      canvas.vertex2(vec2d(1.0-position, 1.0));

      canvas.vertex2(vec2d(position, 0.0));
      canvas.vertex2(vec2d(1.0, 1.0-position));

      // lines with slope = -1
      canvas.vertex2(vec2d(0.0, 1.0-position));
      canvas.vertex2(vec2d(1.0-position, 0.0));

      canvas.vertex2(vec2d(position, 1.0));
      canvas.vertex2(vec2d(1.0, position));
    }

  // final closing lines
  canvas.vertex2(vec2d(0.0, 1.0));
  canvas.vertex2(vec2d(1.0, 1.0));

  canvas.vertex2(vec2d(1.0, 0.0));
  canvas.vertex2(vec2d(1.0, 1.0));
}

static const char __attribute__((used)) vcid_groovx_visx_maskhatch_cc_utc20050626084015[] = "$Id$ $HeadURL$";
#endif // !GROOVX_VISX_MASKHATCH_CC_UTC20050626084015_DEFINED
