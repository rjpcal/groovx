///////////////////////////////////////////////////////////////////////
//
// gxcolor.cc
//
// Copyright (c) 2001-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Wed Aug 22 17:45:52 2001
// written: Wed Mar 19 12:46:02 2003
// $Id$
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
  const IO::VersionId GXCOLOR_SERIAL_VERSION_ID = 1;
}

GxColor::GxColor() :
  FieldContainer(&sigNodeChanged),
  rgbaColor(1.0)
{
  setFieldMap(GxColor::classFields());
}

GxColor::~GxColor() {}

IO::VersionId GxColor::serialVersionId() const
{
  return GXCOLOR_SERIAL_VERSION_ID;
}

void GxColor::readFrom(IO::Reader* reader)
{
  reader->ensureReadVersionId("GxColor", 1,
                              "Try grsh0.8a7");

  readFieldsFrom(reader, classFields());
}

void GxColor::writeTo(IO::Writer* writer) const
{
  writer->ensureWriteVersionId("GxColor", GXCOLOR_SERIAL_VERSION_ID, 1,
                               "Try grsh0.8a7");

  writeFieldsTo(writer, classFields());
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

static const char vcid_gxcolor_cc[] = "$Header$";
#endif // !GXCOLOR_CC_DEFINED
