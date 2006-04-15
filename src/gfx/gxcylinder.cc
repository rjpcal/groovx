/** @file gfx/gxcylinder.cc GxNode subclass for drawing cylinders+cones */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Fri Jul  5 12:46:27 2002
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

#ifndef GROOVX_GFX_GXCYLINDER_CC_UTC20050626084024_DEFINED
#define GROOVX_GFX_GXCYLINDER_CC_UTC20050626084024_DEFINED

#include "gxcylinder.h"

#include "geom/box.h"

#include "gfx/bbox.h"
#include "gfx/canvas.h"

#include "io/reader.h"
#include "io/writer.h"

#include "rutz/trace.h"

namespace
{
  const int GXCYLINDER_SVID = 0;
}

const FieldMap& GxCylinder::classFields()
{
GVX_TRACE("GxCylinder::classFields");
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
GVX_TRACE("GxCylinder::make");
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
GVX_TRACE("GxCylinder::GxCylinder");
  setFieldMap(GxCylinder::classFields());
}

GxCylinder::~GxCylinder() throw()
{
GVX_TRACE("GxCylinder::~GxCylinder");
}

io::version_id GxCylinder::class_version_id() const
{
GVX_TRACE("GxCylinder::class_version_id");
  return GXCYLINDER_SVID;
}

void GxCylinder::read_from(io::reader& reader)
{
GVX_TRACE("GxCylinder::read_from");
  readFieldsFrom(reader, classFields());
}

void GxCylinder::write_to(io::writer& writer) const
{
GVX_TRACE("GxCylinder::write_to");
  writeFieldsTo(writer, classFields(), GXCYLINDER_SVID);
}

/////////////
// actions //
/////////////

void GxCylinder::getBoundingCube(Gfx::Bbox& bbox) const
{
GVX_TRACE("GxCylinder::getBoundingCube");

  geom::box<double> mybox;

  const double maxr = itsBase > itsTop ? itsBase : itsTop;

  mybox.set_xx_yy_zz(-maxr, maxr,
                     -maxr, maxr,
                     0.0, itsHeight);

  bbox.drawBox(mybox);
}

void GxCylinder::draw(Gfx::Canvas& canvas) const
{
GVX_TRACE("GxCylinder::draw");

  canvas.drawCylinder(itsBase, itsTop, itsHeight,
                      itsSlices, itsStacks, itsFilled);
}

static const char __attribute__((used)) vcid_groovx_gfx_gxcylinder_cc_utc20050626084024[] = "$Id$ $HeadURL$";
#endif // !GROOVX_GFX_GXCYLINDER_CC_UTC20050626084024_DEFINED
