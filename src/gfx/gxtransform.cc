/** @file gfx/gxtransform.cc GxNode subclass for transforming the
    modelview matrix, to achieve translation, scaling, and/or
    rotation */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Wed Nov 20 15:15:50 2002
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

#include "gxtransform.h"

#include "geom/box.h"
#include "geom/rect.h"
#include "geom/txform.h"
#include "geom/vec3.h"

#include "gfx/bbox.h"
#include "gfx/canvas.h"

#include "io/reader.h"
#include "io/writer.h"

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

namespace
{
  const io::version_id GXTRANSFORM_SVID = 1;
}

///////////////////////////////////////////////////////////////////////
//
// GxTransformImpl structure
//
///////////////////////////////////////////////////////////////////////

class GxTransformImpl
{
public:
  GxTransformImpl(GxTransform* p) :
    owner(p),
    txformCache(geom::txform::identity())
  {}

  GxTransform* owner;

private:
  mutable geom::txform txformCache;

public:
  const geom::txform& getTxform() const
  {
    txformCache = geom::txform(owner->translation,
                               owner->scaling,
                               owner->rotationAxis,
                               owner->itsRotationAngle);

    return txformCache;
  }
};

///////////////////////////////////////////////////////////////////////
//
// GxTransform member functions
//
///////////////////////////////////////////////////////////////////////

const FieldMap& GxTransform::classFields()
{
  static const Field FIELD_ARRAY[] =
  {
    Field("translation", Field::ValueType(), &GxTransform::translation,
          "0. 0. 0.", "-10. -10. -10.", "10. 10. 10.", "0.1 0.1 0.1",
          Field::NEW_GROUP | Field::MULTI),
    Field("scaling", Field::ValueType(), &GxTransform::scaling,
          "1. 1. 1.", "0.1 0.1 0.1", "10. 10. 10.", "0.1 0.1 0.1",
          Field::MULTI),
    Field("rotationAxis", Field::ValueType(), &GxTransform::rotationAxis,
          "0. 0. 1.", "-1. -1. -1.", "1. 1. 1.", "0.1 0.1 0.1", Field::MULTI),
    Field("rotationAngle",
          &GxTransform::itsRotationAngle, 0., 0., 360., 1.),
    Field("mtxMode", &GxTransform::itsMtxMode, 0, 0, 1, 1,
          Field::TRANSIENT | Field::BOOLEAN),
    Field("jackSize", &GxTransform::itsJackSize, 0, 0, 100, 1, Field::TRANSIENT)
  };

  static FieldMap POS_FIELDS(FIELD_ARRAY);

  return POS_FIELDS;
}

GxTransform* GxTransform::make()
{
GVX_TRACE("GxTransform::make");
  return new GxTransform;
}

GxTransform::GxTransform() :
  FieldContainer(&sigNodeChanged),
  translation(0.0, 0.0, 0.0),
  scaling(1.0, 1.0, 1.0),
  rotationAxis(0.0, 0.0, 1.0),
  itsRotationAngle(0.0),
  itsMtxMode(0),
  itsJackSize(0),
  rep(new GxTransformImpl(this))
{
GVX_TRACE("GxTransform::GxTransform");

  dbg_eval_nl(3, rep);

  setFieldMap(GxTransform::classFields());
}

GxTransform::~GxTransform() noexcept
{
GVX_TRACE("GxTransform::~GxTransform");
  delete rep;
}

io::version_id GxTransform::class_version_id() const
{
GVX_TRACE("GxTransform::class_version_id");
  return GXTRANSFORM_SVID;
}

void GxTransform::read_from(io::reader& reader)
{
GVX_TRACE("GxTransform::read_from");

  reader.ensure_version_id("GxTransform", 1,
                           "Try cvs tag xml_conversion_20040526",
                           SRC_POS);

  readFieldsFrom(reader, classFields());
}

void GxTransform::write_to(io::writer& writer) const
{
GVX_TRACE("GxTransform::write_to");

  writer.ensure_output_version_id("GxTransform",
                              GXTRANSFORM_SVID, 1,
                               "Try groovx0.8a4", SRC_POS);

  writeFieldsTo(writer, classFields(), GXTRANSFORM_SVID);
}

///////////////
// accessors //
///////////////

const geom::txform& GxTransform::getTxform() const
{
GVX_TRACE("GxTransform::getTxform");

  return rep->getTxform();
}

/////////////
// actions //
/////////////

void GxTransform::getBoundingCube(Gfx::Bbox& bbox) const
{
GVX_TRACE("GxTransform::getBoundingCube");

  bbox.transform(getTxform());
}

void GxTransform::draw(Gfx::Canvas& canvas) const
{
GVX_TRACE("GxTransform::draw");

  if (itsMtxMode == 0)
    {
      GVX_TRACE("GxTransform::draw::native-transform");
      canvas.translate(translation);
      canvas.scale(scaling);
      canvas.rotate(rotationAxis, itsRotationAngle);
    }
  else
    {
      GVX_TRACE("GxTransform::draw::custom-transform");
      canvas.transform(rep->getTxform());
    }

  if (itsJackSize > 0)
    {
      Gfx::LinesBlock block(canvas);

      const double d = itsJackSize / 10.0;

      canvas.vertex3(geom::vec3<double>(-d, 0.0, 0.0));
      canvas.vertex3(geom::vec3<double>( d, 0.0, 0.0));

      canvas.vertex3(geom::vec3<double>(0.0, -d, 0.0));
      canvas.vertex3(geom::vec3<double>(0.0,  d, 0.0));

      canvas.vertex3(geom::vec3<double>(0.0, 0.0, -d));
      canvas.vertex3(geom::vec3<double>(0.0, 0.0,  d));
    }
}
