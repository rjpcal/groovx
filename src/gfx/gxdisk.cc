///////////////////////////////////////////////////////////////////////
//
// gxdisk.cc
//
// Copyright (c) 2002-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Tue Nov 26 18:44:29 2002
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef GXDISK_CC_DEFINED
#define GXDISK_CC_DEFINED

#include "gxdisk.h"

#include "gfx/canvas.h"

#include "gx/bbox.h"
#include "gx/box.h"

#include "io/reader.h"
#include "io/writer.h"

#include "util/trace.h"

const FieldMap& GxDisk::classFields()
{
DOTRACE("GxDisk::classFields");
  static const Field FIELD_ARRAY[] =
  {
    Field("innerRadius", &GxDisk::itsInnerRadius, 0.0, 0.0, 100.0, 1.0,
          Field::NEW_GROUP),
    Field("outerRadius", &GxDisk::itsOuterRadius, 1.0, 0.0, 100.0, 1.0),
    Field("slices", &GxDisk::itsNumSlices, 20u, 1u, 100u, 1u),
    Field("loops", &GxDisk::itsNumLoops, 1u, 1u, 100u, 1u),
    Field("filled", &GxDisk::itsFilled, true, false, true, true,
          Field::BOOLEAN)
  };

  static FieldMap FIELD_MAP(FIELD_ARRAY);

  return FIELD_MAP;
}

GxDisk* GxDisk::make()
{
DOTRACE("GxDisk::make");
 return new GxDisk;
}

GxDisk::GxDisk() :
  FieldContainer(&sigNodeChanged),
  itsInnerRadius(0.0),
  itsOuterRadius(1.0),
  itsNumSlices(20),
  itsNumLoops(1),
  itsFilled(true)
{
DOTRACE("GxDisk::GxDisk");
  setFieldMap(GxDisk::classFields());
}

GxDisk::~GxDisk() throw()
{
DOTRACE("GxDisk::~GxDisk");
}

void GxDisk::readFrom(IO::Reader& reader)
{
DOTRACE("GxDisk::readFrom");
  readFieldsFrom(reader, classFields());
}

void GxDisk::writeTo(IO::Writer& writer) const
{
DOTRACE("GxDisk::writeTo");
  writeFieldsTo(writer, classFields());
}

/////////////
// actions //
/////////////

void GxDisk::getBoundingCube(Gfx::Bbox& bbox) const
{
DOTRACE("GxDisk::getBoundingCube");

  Gfx::Box<double> mybox;

  mybox.setXXYYZZ(-itsOuterRadius, itsOuterRadius,
                  -itsOuterRadius, itsOuterRadius,
                  0.0, 0.0);

  bbox.drawBox(mybox);
}

void GxDisk::draw(Gfx::Canvas& canvas) const
{
DOTRACE("GxDisk::draw");

  canvas.drawCircle(itsInnerRadius, itsOuterRadius, itsFilled,
                    itsNumSlices, itsNumLoops);
}

static const char vcid_gxdisk_cc[] = "$Header$";
#endif // !GXDISK_CC_DEFINED
