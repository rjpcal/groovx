///////////////////////////////////////////////////////////////////////
//
// gxsphere.cc
//
// Copyright (c) 2002-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Fri Jul  5 14:20:08 2002
// commit: $Id$
// $HeadURL$
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

#ifndef GROOVX_GFX_GXSPHERE_CC_UTC20050626084025_DEFINED
#define GROOVX_GFX_GXSPHERE_CC_UTC20050626084025_DEFINED

#include "gfx/gxsphere.h"

#include "geom/box.h"

#include "gfx/bbox.h"
#include "gfx/canvas.h"

#include "io/reader.h"
#include "io/writer.h"

#include "rutz/trace.h"

namespace
{
  const int GXSPHERE_SVID = 0;
}

const FieldMap& GxSphere::classFields()
{
GVX_TRACE("GxSphere::classFields");
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
GVX_TRACE("GxSphere::make");
 return new GxSphere;
}

GxSphere::GxSphere() :
  FieldContainer(&sigNodeChanged),
  itsRadius(1.0),
  itsSlices(50),
  itsStacks(50),
  itsFilled(true)
{
GVX_TRACE("GxSphere::GxSphere");
  setFieldMap(GxSphere::classFields());
}

GxSphere::~GxSphere() throw()
{
GVX_TRACE("GxSphere::~GxSphere");
}

IO::VersionId GxSphere::serialVersionId() const
{
GVX_TRACE("GxSphere::serialVersionId");
  return GXSPHERE_SVID;
}

void GxSphere::readFrom(IO::Reader& reader)
{
GVX_TRACE("GxSphere::readFrom");
  readFieldsFrom(reader, classFields());
}

void GxSphere::writeTo(IO::Writer& writer) const
{
GVX_TRACE("GxSphere::writeTo");
  writeFieldsTo(writer, classFields(), GXSPHERE_SVID);
}

/////////////
// actions //
/////////////

void GxSphere::getBoundingCube(Gfx::Bbox& bbox) const
{
GVX_TRACE("GxSphere::getBoundingCube");

  geom::box<double> mybox;

  mybox.set_xx_yy_zz(-itsRadius, itsRadius,
                     -itsRadius, itsRadius,
                     -itsRadius, itsRadius);

  bbox.drawBox(mybox);
}

void GxSphere::draw(Gfx::Canvas& canvas) const
{
GVX_TRACE("GxSphere::draw");

  canvas.drawSphere(itsRadius, itsSlices, itsStacks, itsFilled);
}

static const char vcid_groovx_gfx_gxsphere_cc_utc20050626084025[] = "$Id$ $HeadURL$";
#endif // !GROOVX_GFX_GXSPHERE_CC_UTC20050626084025_DEFINED
