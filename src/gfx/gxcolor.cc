///////////////////////////////////////////////////////////////////////
//
// gxcolor.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Aug 22 17:45:52 2001
// written: Fri Jan 18 16:07:07 2002
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

void GxColor::draw(Gfx::Canvas& canvas) const
{
  canvas.setColor(rgbaColor);
}

void GxColor::undraw(Gfx::Canvas& canvas) const
{
  canvas.setColor(rgbaColor);
}

static const char vcid_gxcolor_cc[] = "$Header$";
#endif // !GXCOLOR_CC_DEFINED
