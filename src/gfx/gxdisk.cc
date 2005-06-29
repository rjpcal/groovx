///////////////////////////////////////////////////////////////////////
//
// gxdisk.cc
//
// Copyright (c) 2002-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Tue Nov 26 18:44:29 2002
// commit: $Id$
// $HeadURL$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
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

#ifndef GROOVX_GFX_GXDISK_CC_UTC20050626084024_DEFINED
#define GROOVX_GFX_GXDISK_CC_UTC20050626084024_DEFINED

#include "gxdisk.h"

#include "geom/box.h"

#include "gfx/bbox.h"
#include "gfx/canvas.h"

#include "io/reader.h"
#include "io/writer.h"

#include "rutz/trace.h"

namespace
{
  const int GXDISK_SVID = 0;
}

const FieldMap& GxDisk::classFields()
{
GVX_TRACE("GxDisk::classFields");
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
GVX_TRACE("GxDisk::make");
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
GVX_TRACE("GxDisk::GxDisk");
  setFieldMap(GxDisk::classFields());
}

GxDisk::~GxDisk() throw()
{
GVX_TRACE("GxDisk::~GxDisk");
}

IO::VersionId GxDisk::serialVersionId() const
{
GVX_TRACE("GxDisk::serialVersionId");
  return GXDISK_SVID;
}

void GxDisk::readFrom(IO::Reader& reader)
{
GVX_TRACE("GxDisk::readFrom");
  readFieldsFrom(reader, classFields());
}

void GxDisk::writeTo(IO::Writer& writer) const
{
GVX_TRACE("GxDisk::writeTo");
  writeFieldsTo(writer, classFields(), GXDISK_SVID);
}

/////////////
// actions //
/////////////

void GxDisk::getBoundingCube(Gfx::Bbox& bbox) const
{
GVX_TRACE("GxDisk::getBoundingCube");

  geom::box<double> mybox;

  mybox.set_xx_yy_zz(-itsOuterRadius, itsOuterRadius,
                     -itsOuterRadius, itsOuterRadius,
                     0.0, 0.0);

  bbox.drawBox(mybox);
}

void GxDisk::draw(Gfx::Canvas& canvas) const
{
GVX_TRACE("GxDisk::draw");

  canvas.drawCircle(itsInnerRadius, itsOuterRadius, itsFilled,
                    itsNumSlices, itsNumLoops);
}

static const char vcid_groovx_gfx_gxdisk_cc_utc20050626084024[] = "$Id$ $HeadURL$";
#endif // !GROOVX_GFX_GXDISK_CC_UTC20050626084024_DEFINED
