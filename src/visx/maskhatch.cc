///////////////////////////////////////////////////////////////////////
//
// maskhatch.cc
//
// Copyright (c) 1999-2003 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Sep 23 15:49:58 1999
// written: Mon Jan 13 11:08:25 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef MASKHATCH_CC_DEFINED
#define MASKHATCH_CC_DEFINED

#include "visx/maskhatch.h"

#include "gfx/canvas.h"
#include "gfx/gxaligner.h"

#include "gx/bbox.h"
#include "gx/rect.h"

#include "io/ioproxy.h"
#include "io/reader.h"
#include "io/writer.h"

#include "util/trace.h"
#include "util/debug.h"

namespace
{
  const IO::VersionId MASKHATCH_SERIAL_VERSION_ID = 2;
}

const FieldMap& MaskHatch::classFields()
{
  static const Field MASK_FINFOS[] =
  {
    Field("numLines", &MaskHatch::itsNumLines, 5, 0, 25, 1, Field::NEW_GROUP),
    Field("lineWidth", &MaskHatch::itsLineWidth, 1, 0, 25, 1, Field::CHECKED)
  };

  static FieldMap MASK_FIELDS(MASK_FINFOS, &GxShapeKit::classFields());

  return MASK_FIELDS;
}

MaskHatch* MaskHatch::make()
{
DOTRACE("MaskHatch::make");
  return new MaskHatch;
}

MaskHatch::MaskHatch () :
  GxShapeKit(),
  itsNumLines(10),
  itsLineWidth(1)
{
DOTRACE("MaskHatch::MaskHatch");

  sigNodeChanged.connect(this, &MaskHatch::update);

  setFieldMap(MaskHatch::classFields());

  setAlignmentMode(GxAligner::CENTER_ON_CENTER);
  dbgEval(3, getAlignmentMode());
}

MaskHatch::~MaskHatch()
{
DOTRACE("MaskHatch::~MaskHatch");
}

IO::VersionId MaskHatch::serialVersionId() const
{
DOTRACE("MaskHatch::serialVersionId");
 return MASKHATCH_SERIAL_VERSION_ID;
}

void MaskHatch::readFrom(IO::Reader* reader)
{
DOTRACE("MaskHatch::readFrom");

  reader->ensureReadVersionId("MaskHatch", 2, "Try grsh0.8a4");

  readFieldsFrom(reader, classFields());

  // FIXME change to "GxShapeKit" with next version
  reader->readBaseClass("GrObj", IO::makeProxy<GxShapeKit>(this));
}

void MaskHatch::writeTo(IO::Writer* writer) const
{
DOTRACE("MaskHatch::writeTo");

  writer->ensureWriteVersionId("MaskHatch", MASKHATCH_SERIAL_VERSION_ID, 2,
                               "Try grsh0.8a4");

  writeFieldsTo(writer, classFields());

  // FIXME change to "GxShapeKit" with next version
  writer->writeBaseClass("GrObj", IO::makeConstProxy<GxShapeKit>(this));
}

void MaskHatch::update()
{
  setPixelBorder(itsLineWidth/2 + 2);
}

void MaskHatch::grGetBoundingBox(Gfx::Bbox& bbox) const
{
DOTRACE("MaskHatch::grGetBoundingBox");

  bbox.drawRect(Gfx::RectLBWH<double>(0.0, 0.0, 1.0, 1.0));
}

void MaskHatch::grRender(Gfx::Canvas& canvas) const
{
DOTRACE("MaskHatch::grRender");

  if (itsNumLines == 0) return;

  Gfx::AttribSaver attribSaver(canvas);

  canvas.setLineWidth(itsLineWidth);

  Gfx::LinesBlock b(canvas);

  for (int i = 0; i < itsNumLines; ++i)
    {
      double position = double(i)/itsNumLines;

      // horizontal line
      canvas.vertex2(Gfx::Vec2<double>(0.0, position));
      canvas.vertex2(Gfx::Vec2<double>(1.0, position));

      // vertical line
      canvas.vertex2(Gfx::Vec2<double>(position, 0.0));
      canvas.vertex2(Gfx::Vec2<double>(position, 1.0));

      // lines with slope = 1
      canvas.vertex2(Gfx::Vec2<double>(0.0, position));
      canvas.vertex2(Gfx::Vec2<double>(1.0-position, 1.0));

      canvas.vertex2(Gfx::Vec2<double>(position, 0.0));
      canvas.vertex2(Gfx::Vec2<double>(1.0, 1.0-position));

      // lines with slope = -1
      canvas.vertex2(Gfx::Vec2<double>(0.0, 1.0-position));
      canvas.vertex2(Gfx::Vec2<double>(1.0-position, 0.0));

      canvas.vertex2(Gfx::Vec2<double>(position, 1.0));
      canvas.vertex2(Gfx::Vec2<double>(1.0, position));
    }

  // final closing lines
  canvas.vertex2(Gfx::Vec2<double>(0.0, 1.0));
  canvas.vertex2(Gfx::Vec2<double>(1.0, 1.0));

  canvas.vertex2(Gfx::Vec2<double>(1.0, 0.0));
  canvas.vertex2(Gfx::Vec2<double>(1.0, 1.0));
}

static const char vcid_maskhatch_cc[] = "$Header$";
#endif // !MASKHATCH_CC_DEFINED
