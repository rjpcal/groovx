///////////////////////////////////////////////////////////////////////
//
// gxcylinder.cc
//
// Copyright (c) 2002-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Fri Jul  5 12:46:27 2002
// written: Wed Mar 19 17:56:04 2003
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

#ifndef GXCYLINDER_CC_DEFINED
#define GXCYLINDER_CC_DEFINED

#include "gfx/gxcylinder.h"

#include "gfx/canvas.h"

#include "gx/bbox.h"
#include "gx/box.h"

#include "io/reader.h"
#include "io/writer.h"

#include "util/trace.h"

const FieldMap& GxCylinder::classFields()
{
DOTRACE("GxCylinder::classFields");
  static const Field FIELD_ARRAY[] =
  {
    Field("base", &GxCylinder::itsBase, 1.0, 0.0, 10.0, 0.1,
          Field::NEW_GROUP),
    Field("top", &GxCylinder::itsTop, 1.0, 0.0, 10.0, 0.1),
    Field("height", &GxCylinder::itsHeight, 1.0, 0.0, 10.0, 0.1),
    Field("slices", &GxCylinder::itsSlices, 10, 1, 100, 1),
    Field("stacks", &GxCylinder::itsStacks, 10, 1, 100, 1),
    Field("filled", &GxCylinder::itsFilled, true, false, true, true,
          Field::BOOLEAN)
  };

  static FieldMap FIELD_MAP(FIELD_ARRAY);

  return FIELD_MAP;
}

GxCylinder* GxCylinder::make()
{
DOTRACE("GxCylinder::make");
 return new GxCylinder;
}

GxCylinder::GxCylinder() :
  FieldContainer(&sigNodeChanged),
  itsBase(1.0),
  itsTop(1.0),
  itsHeight(1.0),
  itsSlices(75),
  itsStacks(20),
  itsFilled(true)
{
DOTRACE("GxCylinder::GxCylinder");
  setFieldMap(GxCylinder::classFields());
}

GxCylinder::~GxCylinder()
{
DOTRACE("GxCylinder::~GxCylinder");
}

void GxCylinder::readFrom(IO::Reader* reader)
{
DOTRACE("GxCylinder::readFrom");
  readFieldsFrom(reader, classFields());
}

void GxCylinder::writeTo(IO::Writer* writer) const
{
DOTRACE("GxCylinder::writeTo");
  writeFieldsTo(writer, classFields());
}

/////////////
// actions //
/////////////

void GxCylinder::getBoundingCube(Gfx::Bbox& bbox) const
{
DOTRACE("GxCylinder::getBoundingCube");

  Gfx::Box<double> mybox;

  const double maxr = itsBase > itsTop ? itsBase : itsTop;

  mybox.setXXYYZZ(-maxr, maxr,
                  -maxr, maxr,
                  0.0, itsHeight);

  bbox.drawBox(mybox);
}

void GxCylinder::draw(Gfx::Canvas& canvas) const
{
DOTRACE("GxCylinder::draw");

  canvas.drawCylinder(itsBase, itsTop, itsHeight,
                      itsSlices, itsStacks, itsFilled);
}

static const char vcid_gxcylinder_cc[] = "$Header$";
#endif // !GXCYLINDER_CC_DEFINED
