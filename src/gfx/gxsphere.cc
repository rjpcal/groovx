///////////////////////////////////////////////////////////////////////
//
// gxsphere.cc
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Jul  5 14:20:08 2002
// written: Fri Jul  5 14:25:26 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXSPHERE_CC_DEFINED
#define GXSPHERE_CC_DEFINED

#include "gfx/gxsphere.h"

#include "gfx/canvas.h"

#include "gx/box.h"

#include "io/reader.h"
#include "io/writer.h"

#include "util/trace.h"

const FieldMap& GxSphere::classFields()
{
DOTRACE("GxSphere::classFields");
  static const Field FIELD_ARRAY[] =
  {
    Field("radius", &GxSphere::itsRadius, 1.0, 0.0, 10.0, 0.1,
          Field::NEW_GROUP),
    Field("slices", &GxSphere::itsSlices, 10, 1, 100, 1),
    Field("stacks", &GxSphere::itsStacks, 10, 1, 100, 1),
    Field("filled", &GxSphere::itsFilled, true, false, true, true,
          Field::BOOLEAN)
  };

  static FieldMap FIELD_MAP(FIELD_ARRAY);

  return FIELD_MAP;
}

GxSphere* GxSphere::make()
{
DOTRACE("GxSphere::make");
 return new GxSphere;
}

GxSphere::GxSphere() :
  FieldContainer(&sigNodeChanged),
  itsRadius(1.0),
  itsSlices(50),
  itsStacks(50),
  itsFilled(true)
{
DOTRACE("GxSphere::GxSphere");
  setFieldMap(GxSphere::classFields());
}

GxSphere::~GxSphere()
{
DOTRACE("GxSphere::~GxSphere");
}

void GxSphere::readFrom(IO::Reader* reader)
{
DOTRACE("GxSphere::readFrom");
  readFieldsFrom(reader, classFields());
}

void GxSphere::writeTo(IO::Writer* writer) const
{
DOTRACE("GxSphere::writeTo");
  writeFieldsTo(writer, classFields());
}

/////////////
// actions //
/////////////

void GxSphere::getBoundingCube(Gfx::Box<double>& cube, Gfx::Canvas&) const
{
DOTRACE("GxSphere::getBoundingCube");

  Gfx::Box<double> mybox;

  mybox.setXXYYZZ(-itsRadius, itsRadius,
                  -itsRadius, itsRadius,
                  -itsRadius, itsRadius);

  cube.unionize(mybox);
}

void GxSphere::draw(Gfx::Canvas& canvas) const
{
DOTRACE("GxSphere::draw");

  canvas.drawSphere(itsRadius, itsSlices, itsStacks, itsFilled);
}

static const char vcid_gxsphere_cc[] = "$Header$";
#endif // !GXSPHERE_CC_DEFINED
