///////////////////////////////////////////////////////////////////////
//
// gxdrawstyle.cc
//
// Copyright (c) 2001-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Wed Aug 29 10:19:31 2001
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

#ifndef GXDRAWSTYLE_CC_DEFINED
#define GXDRAWSTYLE_CC_DEFINED

#include "gfx/gxdrawstyle.h"

#include "gfx/canvas.h"

#include "io/reader.h"
#include "io/writer.h"

namespace
{
  const IO::VersionId GXDRAWSTYLE_SERIAL_VERSION_ID = 1;
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

GxDrawStyle::~GxDrawStyle() throw() {}

IO::VersionId GxDrawStyle::serialVersionId() const
{
  return GXDRAWSTYLE_SERIAL_VERSION_ID;
}

void GxDrawStyle::readFrom(IO::Reader& reader)
{
  reader.ensureReadVersionId("GxDrawStyle", 1,
                             "Try grsh0.8a7");

  readFieldsFrom(reader, classFields());
}

void GxDrawStyle::writeTo(IO::Writer& writer) const
{
  writer.ensureWriteVersionId("GxDrawStyle", GXDRAWSTYLE_SERIAL_VERSION_ID, 1,
                              "Try grsh0.8a7");

  writeFieldsTo(writer, classFields());
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
  canvas.setLineStipple((unsigned short)linePattern);
}

static const char vcid_gxdrawstyle_cc[] = "$Header$";
#endif // !GXDRAWSTYLE_CC_DEFINED
