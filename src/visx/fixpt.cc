///////////////////////////////////////////////////////////////////////
//
// fixpt.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jan  4 08:00:00 1999
// written: Tue Nov 19 13:41:18 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FIXPT_CC_DEFINED
#define FIXPT_CC_DEFINED

#include "visx/fixpt.h"

#include "gfx/canvas.h"

#include "gx/bbox.h"

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

void FixPt::grGetBoundingBox(Gfx::Bbox& bbox) const
{
DOTRACE("FixPt::grGetBoundingBox");

  bbox.cube.merge(Gfx::Vec2<double>(-itsLength/2.0, -itsLength/2.0));
  bbox.cube.merge(Gfx::Vec2<double>(+itsLength/2.0, +itsLength/2.0));
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
