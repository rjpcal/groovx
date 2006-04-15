/** @file gfx/gxline.cc GxNode subclass for drawing lines */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Wed Apr 17 16:00:32 2002
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

#ifndef GROOVX_GFX_GXLINE_CC_UTC20050626084025_DEFINED
#define GROOVX_GFX_GXLINE_CC_UTC20050626084025_DEFINED

#include "gxline.h"

#include "geom/box.h"

#include "gfx/bbox.h"
#include "gfx/canvas.h"

#include "io/reader.h"
#include "io/writer.h"

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

namespace
{
  const int GXLINE_SVID = 0;
}

///////////////////////////////////////////////////////////////////////
//
// GxLine member definitions
//
///////////////////////////////////////////////////////////////////////

const FieldMap& GxLine::classFields()
{
GVX_TRACE("GxLine::classFields");
  static const Field FIELD_ARRAY[] =
  {
    Field("start", Field::ValueType(), &GxLine::start,
          "0. 0. 0.", "-10. -10. -10.", "10. 10. 10.", "0.1 0.1 0.1",
          Field::NEW_GROUP | Field::MULTI),
    Field("stop", Field::ValueType(), &GxLine::stop,
          "0. 0. 0.", "-10. -10. -10.", "10. 10. 10.", "0.1 0.1 0.1",
          Field::MULTI),
    Field("width", &GxLine::itsWidth, 1, 1, 30, 1)
  };

  static FieldMap FIELD_MAP(FIELD_ARRAY);

  return FIELD_MAP;
}

GxLine* GxLine::make()
{
GVX_TRACE("GxLine::make");
 return new GxLine;
}

GxLine::GxLine() :
  FieldContainer(&sigNodeChanged),
  start(0., 0., 0.),
  stop(0., 0., 0.),
  itsWidth(1)
{
GVX_TRACE("GxLine::GxLine");
  setFieldMap(GxLine::classFields());
}

GxLine::~GxLine() throw()
{
GVX_TRACE("GxLine::~GxLine");
}

io::version_id GxLine::class_version_id() const
{
GVX_TRACE("GxLine::class_version_id");
  return GXLINE_SVID;
}

void GxLine::read_from(io::reader& reader)
{
GVX_TRACE("GxLine::read_from");
  readFieldsFrom(reader, classFields());
}

void GxLine::write_to(io::writer& writer) const
{
GVX_TRACE("GxLine::write_to");
  writeFieldsTo(writer, classFields(), GXLINE_SVID);
}

/////////////
// actions //
/////////////

void GxLine::getBoundingCube(Gfx::Bbox& bbox) const
{
GVX_TRACE("GxColor::getBoundingCube");

  bbox.vertex3(start);
  bbox.vertex3(stop);
}

void GxLine::draw(Gfx::Canvas& canvas) const
{
GVX_TRACE("GxLine::draw");

  Gfx::AttribSaver as(canvas);

  if (itsWidth > 0)
    canvas.setLineWidth(itsWidth);

  Gfx::LinesBlock lb(canvas);
  canvas.vertex3(start.vec());
  canvas.vertex3(stop.vec());
}

static const char __attribute__((used)) vcid_groovx_gfx_gxline_cc_utc20050626084025[] = "$Id$ $HeadURL$";
#endif // !GROOVX_GFX_GXLINE_CC_UTC20050626084025_DEFINED
