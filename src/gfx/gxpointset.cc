///////////////////////////////////////////////////////////////////////
//
// gxpointset.cc
//
// Copyright (c) 2002-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Fri Jul  5 12:46:27 2002
// written: Wed Mar 19 12:46:00 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXPOINTSET_CC_DEFINED
#define GXPOINTSET_CC_DEFINED

#include "gfx/gxpointset.h"

#include "gfx/canvas.h"

#include "gx/bbox.h"
#include "gx/box.h"

#include "io/reader.h"
#include "io/writer.h"

#include "util/trace.h"

const FieldMap& GxPointSet::classFields()
{
DOTRACE("GxPointSet::classFields");
  static const Field FIELD_ARRAY[] =
  {
    Field("points", Field::ValueType(), &GxPointSet::itsPoints,
          "", "", "", "", Field::NEW_GROUP | Field::STRING),
    Field("style", &GxPointSet::itsStyle, 0, 0, 10, 1),
  };

  static FieldMap FIELD_MAP(FIELD_ARRAY);

  return FIELD_MAP;
}

GxPointSet* GxPointSet::make()
{
DOTRACE("GxPointSet::make");
 return new GxPointSet;
}

GxPointSet::GxPointSet() :
  FieldContainer(&sigNodeChanged),
  itsPoints()
{
DOTRACE("GxPointSet::GxPointSet");
  setFieldMap(GxPointSet::classFields());
}

GxPointSet::~GxPointSet()
{
DOTRACE("GxPointSet::~GxPointSet");
}

void GxPointSet::readFrom(IO::Reader* reader)
{
DOTRACE("GxPointSet::readFrom");
  readFieldsFrom(reader, classFields());
}

void GxPointSet::writeTo(IO::Writer* writer) const
{
DOTRACE("GxPointSet::writeTo");
  writeFieldsTo(writer, classFields());
}

/////////////
// actions //
/////////////

void GxPointSet::getBoundingCube(Gfx::Bbox& bbox) const
{
DOTRACE("GxPointSet::getBoundingCube");

  for (unsigned int i = 0; i < itsPoints.arr().size(); ++i)
    bbox.vertex3(itsPoints.arr()[i]);
}

void GxPointSet::draw(Gfx::Canvas& canvas) const
{
DOTRACE("GxPointSet::draw");

  canvas.begin(Gfx::Canvas::VertexStyle(itsStyle));

  for (unsigned int i = 0; i < itsPoints.arr().size(); ++i)
    canvas.vertex3(itsPoints.arr()[i]);

  canvas.end();
}

static const char vcid_gxpointset_cc[] = "$Header$";
#endif // !GXPOINTSET_CC_DEFINED
