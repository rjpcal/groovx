///////////////////////////////////////////////////////////////////////
//
// fixpt.cc
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Mon Jan  4 08:00:00 1999
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef FIXPT_CC_DEFINED
#define FIXPT_CC_DEFINED

#include "visx/fixpt.h"

#include "gfx/canvas.h"

#include "gx/bbox.h"
#include "gx/vec2.h"

#include "io/ioproxy.h"
#include "io/reader.h"
#include "io/writer.h"

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER;

///////////////////////////////////////////////////////////////////////
//
// File scope data
//
///////////////////////////////////////////////////////////////////////

namespace
{
  const IO::VersionId FIXPT_SERIAL_VERSION_ID = 2;
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
DOTRACE("FixPt::make");
  return new FixPt;
}

FixPt::FixPt(double len, int wid) :
  itsLength(len), itsWidth(wid)
{
  setFieldMap(FixPt::classFields());
}

FixPt::~FixPt() throw() {}

IO::VersionId FixPt::serialVersionId() const
{
DOTRACE("FixPt::serialVersionId");
  return FIXPT_SERIAL_VERSION_ID;
}

void FixPt::readFrom(IO::Reader& reader)
{
DOTRACE("FixPt::readFrom");

  reader.ensureReadVersionId("FixPt", 2, "Try grsh0.8a4");

  readFieldsFrom(reader, classFields());

  // FIXME change to "GxShapeKit" with next version
  reader.readBaseClass("GrObj", IO::makeProxy<GxShapeKit>(this));
}

void FixPt::writeTo(IO::Writer& writer) const
{
DOTRACE("FixPt::writeTo");

  writer.ensureWriteVersionId("FixPt", FIXPT_SERIAL_VERSION_ID, 2,
                              "Try grsh0.8a4");

  writeFieldsTo(writer, classFields());

  // FIXME change to "GxShapeKit" with next version
  writer.writeBaseClass("GrObj", IO::makeConstProxy<GxShapeKit>(this));
}

void FixPt::grGetBoundingBox(Gfx::Bbox& bbox) const
{
DOTRACE("FixPt::grGetBoundingBox");

  bbox.vertex2(Gfx::Vec2<double>(-itsLength/2.0, -itsLength/2.0));
  bbox.vertex2(Gfx::Vec2<double>(+itsLength/2.0, +itsLength/2.0));
}

void FixPt::grRender(Gfx::Canvas& canvas) const
{
DOTRACE("FixPt::grRender");

  Gfx::AttribSaver saver(canvas);

  canvas.setLineWidth(itsWidth);

  {
    Gfx::LinesBlock block(canvas);

    canvas.vertex2(Gfx::Vec2<double>(0.0, -itsLength/2.0));
    canvas.vertex2(Gfx::Vec2<double>(0.0, itsLength/2.0));

    canvas.vertex2(Gfx::Vec2<double>(-itsLength/2.0, 0.0));
    canvas.vertex2(Gfx::Vec2<double>(itsLength/2.0, 0.0));
  }
}

static const char vcid_fixpt_cc[] = "$Header$";
#endif // !FIXPT_CC_DEFINED
