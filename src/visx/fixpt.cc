///////////////////////////////////////////////////////////////////////
//
// fixpt.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jan  4 08:00:00 1999
// written: Wed Sep  5 17:31:12 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FIXPT_CC_DEFINED
#define FIXPT_CC_DEFINED

#include "fixpt.h"

#include "gfx/canvas.h"
#include "gfx/rect.h"

#include "io/ioproxy.h"
#include "io/reader.h"
#include "io/writer.h"

#include "util/trace.h"
#include "util/debug.h"

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

  static FieldMap FIXPT_FIELDS(FIELD_ARRAY, &GrObj::classFields());

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

FixPt::~FixPt() {}

IO::VersionId FixPt::serialVersionId() const
{
DOTRACE("FixPt::serialVersionId");
  return FIXPT_SERIAL_VERSION_ID;
}

void FixPt::readFrom(IO::Reader* reader)
{
DOTRACE("FixPt::readFrom");

  reader->ensureReadVersionId("FixPt", 2, "Try grsh0.8a4");

  readFieldsFrom(reader, classFields());

  reader->readBaseClass("GrObj", IO::makeProxy<GrObj>(this));
}

void FixPt::writeTo(IO::Writer* writer) const
{
DOTRACE("FixPt::writeTo");

  writer->ensureWriteVersionId("FixPt", FIXPT_SERIAL_VERSION_ID, 2,
                               "Try grsh0.8a4");

  writeFieldsTo(writer, classFields());

  writer->writeBaseClass("GrObj", IO::makeConstProxy<GrObj>(this));
}

Gfx::Rect<double> FixPt::grGetBoundingBox(Gfx::Canvas&) const
{
DOTRACE("FixPt::grGetBoundingBox");

  return Gfx::RectLBWH<double>(-itsLength/2.0, -itsLength/2.0,
                               itsLength, itsLength);
}

void FixPt::grRender(Gfx::Canvas& canvas) const
{
DOTRACE("FixPt::grRender");

  Gfx::Canvas::AttribSaver saver(canvas);

  canvas.setLineWidth(itsWidth);

  {
    Gfx::Canvas::LinesBlock block(canvas);

    canvas.vertex2(Gfx::Vec2<double>(0.0, -itsLength/2.0));
    canvas.vertex2(Gfx::Vec2<double>(0.0, itsLength/2.0));

    canvas.vertex2(Gfx::Vec2<double>(-itsLength/2.0, 0.0));
    canvas.vertex2(Gfx::Vec2<double>(itsLength/2.0, 0.0));
  }
}

static const char vcid_fixpt_cc[] = "$Header$";
#endif // !FIXPT_CC_DEFINED
