///////////////////////////////////////////////////////////////////////
//
// gxdrawstyle.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Aug 29 10:19:31 2001
// written: Wed Aug 29 16:49:14 2001
// $Id$
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

GxDrawStyle::~GxDrawStyle() {}

IO::VersionId GxDrawStyle::serialVersionId() const
{
  return GXDRAWSTYLE_SERIAL_VERSION_ID;
}

void GxDrawStyle::readFrom(IO::Reader* reader)
{
  reader->ensureReadVersionId("GxDrawStyle", 1,
                              "Try grsh0.8a7");

  readFieldsFrom(reader, classFields());
}

void GxDrawStyle::writeTo(IO::Writer* writer) const
{
  writer->ensureWriteVersionId("GxDrawStyle", GXDRAWSTYLE_SERIAL_VERSION_ID, 1,
                               "Try grsh0.8a7");

  writeFieldsTo(writer, classFields());
}

const FieldMap& GxDrawStyle::classFields()
{
  static const Field FIELD_ARRAY[] =
  {
    Field("polygonFill", &GxDrawStyle::polygonFill, false, false, true, true,
          Field::NEW_GROUP),
    Field("pointSize", &GxDrawStyle::pointSize, 1.0, 0.1, 1.0, 0.1),
    Field("lineWidth", &GxDrawStyle::lineWidth, 1.0, 0.1, 1.0, 0.1),
    Field("linePattern", &GxDrawStyle::linePattern, 0xFFFF, 0, 0xFFFF, 1)
  };

  static FieldMap GXDRAWSTYLE_FIELDS(FIELD_ARRAY);

  return GXDRAWSTYLE_FIELDS;
}

void GxDrawStyle::draw(Gfx::Canvas& canvas) const
{
  canvas.setPolygonFill(polygonFill);
  canvas.setPointSize(pointSize);
  canvas.setLineWidth(lineWidth);
  canvas.setLineStipple((unsigned short)linePattern);
}

void GxDrawStyle::undraw(Gfx::Canvas& canvas) const
{
  canvas.setPolygonFill(polygonFill);
  canvas.setPointSize(pointSize);
  canvas.setLineWidth(lineWidth);
  canvas.setLineStipple((unsigned short)linePattern);
}

static const char vcid_gxdrawstyle_cc[] = "$Header$";
#endif // !GXDRAWSTYLE_CC_DEFINED
