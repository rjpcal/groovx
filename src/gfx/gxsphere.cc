///////////////////////////////////////////////////////////////////////
//
// gxsphere.cc
//
// Copyright (c) 2002-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Fri Jul  5 14:20:08 2002
// written: Wed Mar 19 17:55:59 2003
// $Id$
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

#ifndef GXSPHERE_CC_DEFINED
#define GXSPHERE_CC_DEFINED

#include "gfx/gxsphere.h"

#include "gfx/canvas.h"

#include "gx/bbox.h"
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

void GxSphere::getBoundingCube(Gfx::Bbox& bbox) const
{
DOTRACE("GxSphere::getBoundingCube");

  Gfx::Box<double> mybox;

  mybox.setXXYYZZ(-itsRadius, itsRadius,
                  -itsRadius, itsRadius,
                  -itsRadius, itsRadius);

  bbox.drawBox(mybox);
}

void GxSphere::draw(Gfx::Canvas& canvas) const
{
DOTRACE("GxSphere::draw");

  canvas.drawSphere(itsRadius, itsSlices, itsStacks, itsFilled);
}

static const char vcid_gxsphere_cc[] = "$Header$";
#endif // !GXSPHERE_CC_DEFINED
