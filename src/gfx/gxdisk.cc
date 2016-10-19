/** @file gfx/gxdisk.cc graphic object for circles/rings/disks */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Tue Nov 26 18:44:29 2002
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

GxDisk::~GxDisk() noexcept
{
GVX_TRACE("GxDisk::~GxDisk");
}

io::version_id GxDisk::class_version_id() const
{
GVX_TRACE("GxDisk::class_version_id");
  return GXDISK_SVID;
}

void GxDisk::read_from(io::reader& reader)
{
GVX_TRACE("GxDisk::read_from");
  readFieldsFrom(reader, classFields());
}

void GxDisk::write_to(io::writer& writer) const
{
GVX_TRACE("GxDisk::write_to");
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
