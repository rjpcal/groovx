///////////////////////////////////////////////////////////////////////
//
// gxline.cc
//
// Copyright (c) 2002-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Wed Apr 17 16:00:32 2002
// written: Wed Mar 19 17:56:01 2003
// $Id$
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

#ifndef GXLINE_CC_DEFINED
#define GXLINE_CC_DEFINED

#include "gxline.h"

#include "gfx/canvas.h"

#include "gx/bbox.h"
#include "gx/box.h"

#include "io/reader.h"
#include "io/writer.h"

#include "util/trace.h"
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// GxLine member definitions
//
///////////////////////////////////////////////////////////////////////

const FieldMap& GxLine::classFields()
{
DOTRACE("GxLine::classFields");
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
DOTRACE("GxLine::make");
 return new GxLine;
}

GxLine::GxLine() :
  FieldContainer(&sigNodeChanged),
  start(0., 0., 0.),
  stop(0., 0., 0.),
  itsWidth(1)
{
DOTRACE("GxLine::GxLine");
  setFieldMap(GxLine::classFields());
}

GxLine::~GxLine()
{
DOTRACE("GxLine::~GxLine");
}

void GxLine::readFrom(IO::Reader* reader)
{
DOTRACE("GxLine::readFrom");
  readFieldsFrom(reader, classFields());
}

void GxLine::writeTo(IO::Writer* writer) const
{
DOTRACE("GxLine::writeTo");
  writeFieldsTo(writer, classFields());
}

/////////////
// actions //
/////////////

void GxLine::getBoundingCube(Gfx::Bbox& bbox) const
{
DOTRACE("GxColor::getBoundingCube");

  bbox.vertex3(start);
  bbox.vertex3(stop);
}

void GxLine::draw(Gfx::Canvas& canvas) const
{
DOTRACE("GxLine::draw");

  Gfx::AttribSaver as(canvas);

  if (itsWidth > 0)
    canvas.setLineWidth(itsWidth);

  Gfx::LinesBlock lb(canvas);
  canvas.vertex3(start.vec());
  canvas.vertex3(stop.vec());
}

static const char vcid_gxline_cc[] = "$Header$";
#endif // !GXLINE_CC_DEFINED
