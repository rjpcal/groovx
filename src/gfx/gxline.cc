///////////////////////////////////////////////////////////////////////
//
// gxline.cc
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Apr 17 16:00:32 2002
// written: Fri Jul  5 13:52:15 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXLINE_CC_DEFINED
#define GXLINE_CC_DEFINED

#include "gxline.h"

#include "gfx/canvas.h"

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

void GxLine::getBoundingCube(Gfx::Box<double>& box, Gfx::Canvas&) const
{
DOTRACE("GxColor::getBoundingCube");

  Gfx::Box<double> mybox;

  mybox.setCorners(start, stop);

  box.unionize(mybox);
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

void GxLine::undraw(Gfx::Canvas& canvas) const
{
DOTRACE("GxLine::undraw");
  draw(canvas);
}

static const char vcid_gxline_cc[] = "$Header$";
#endif // !GXLINE_CC_DEFINED
